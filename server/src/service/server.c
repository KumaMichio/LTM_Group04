// Multi-client TCP server using epoll
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>

#include "service/server.h"
#include "service/client_session.h"
#include "service/session_manager.h"
#include "service/dispatcher.h"
#include "service/protocol.h"
#include "service/quickmode_service.h"
#include "service/friends_service.h"
#include "service/reconnect_manager.h"
#include "service/onevn_service.h"
#include "dao/dao_sessions.h"
#include "utils/timer.h"

static volatile int running = 1;

static void handle_sigint(int signum) {
	(void)signum;
	running = 0;
}

static int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int start_server(const char *bind_addr, const char *portstr) {
	struct addrinfo hints, *res, *rp;
	int sockfd = -1;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int s = getaddrinfo(bind_addr, portstr, &hints, &res);
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		return -1;
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) {
		sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sockfd == -1) continue;

		int opt = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

		if (bind(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) break;
		close(sockfd);
		sockfd = -1;
	}
	freeaddrinfo(res);

	if (sockfd < 0) {
		fprintf(stderr, "Failed to bind socket\n");
		return -1;
	}

	// Set non-blocking
	if (set_nonblocking(sockfd) < 0) {
		fprintf(stderr, "Failed to set non-blocking\n");
		close(sockfd);
		return -1;
	}

	if (listen(sockfd, 16) != 0) {
		fprintf(stderr, "listen failed: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	signal(SIGINT, handle_sigint);

	// Create session manager
	SessionManager *mgr = session_manager_new(MAX_SESSIONS);
	if (!mgr) {
		fprintf(stderr, "Failed to create session manager\n");
		close(sockfd);
		return -1;
	}

	// Set global session manager (important for session_manager_get_by_user_id)
	session_manager_set_global(mgr);

	// Add server socket to epoll
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = sockfd;
	if (epoll_ctl(session_manager_get_epoll_fd(mgr), EPOLL_CTL_ADD, sockfd, &ev) < 0) {
		fprintf(stderr, "epoll_ctl failed: %s\n", strerror(errno));
		session_manager_free(mgr);
		close(sockfd);
		return -1;
	}

	printf("Server listening on %s:%s (epoll-based, multi-client)\n", 
	       bind_addr ? bind_addr : "0.0.0.0", portstr);

	struct epoll_event events[MAX_EPOLL_EVENTS];
	time_t last_stale_cleanup = time(NULL);
	const int STALE_CLEANUP_INTERVAL = 30;  // Run cleanup every 30 seconds
	const int STALE_SESSION_TIMEOUT = 300;  // Mark sessions stale if no heartbeat for 5 minutes

	while (running) {
		// Check and run expired game timers (for 1vN mode timeout handling)
		game_timer_check_and_run();
		
		// [HEARTBEAT] Periodic cleanup of stale sessions
		time_t now = time(NULL);
		if (now - last_stale_cleanup >= STALE_CLEANUP_INTERVAL) {
			// Find stale users first
			int64_t *stale_user_ids = NULL;
			int stale_count = dao_sessions_find_stale_users(STALE_SESSION_TIMEOUT, &stale_user_ids);
			
			if (stale_count > 0 && stale_user_ids) {
				// Force disconnect each stale user
				for (int i = 0; i < stale_count; i++) {
					int64_t user_id = stale_user_ids[i];
					ClientSession *sess = session_manager_get_by_user_id(user_id);
					
					if (sess) {
						printf("[HEARTBEAT] Force disconnecting stale session: user_id=%ld (no activity for %d seconds)\n",
						       user_id, STALE_SESSION_TIMEOUT);
						
						// Send error notification before closing
						protocol_send_error(sess, 0x0803, "Session_Timeout");
						
						// Close socket to trigger client disconnect
						close(sess->socket_fd);
					}
				}
				free(stale_user_ids);
			}
			
			// Now cleanup stale sessions in database
			dao_sessions_cleanup_stale(STALE_SESSION_TIMEOUT);
			last_stale_cleanup = now;
		}
		
		int nfds = epoll_wait(session_manager_get_epoll_fd(mgr), events, MAX_EPOLL_EVENTS, 100);
		
		if (nfds < 0) {
			if (errno == EINTR) continue;
			fprintf(stderr, "epoll_wait failed: %s\n", strerror(errno));
			break;
		}

		for (int i = 0; i < nfds; i++) {
			int fd = events[i].data.fd;

			// New connection
			if (fd == sockfd) {
				// Accept all pending connections (edge-triggered)
				while (1) {
					struct sockaddr_storage cli_addr;
					socklen_t cli_len = sizeof(cli_addr);
					int client_fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
					
					if (client_fd < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							// No more connections
							break;
						}
						fprintf(stderr, "accept failed: %s\n", strerror(errno));
						break;
					}

					// Set non-blocking
					if (set_nonblocking(client_fd) < 0) {
						close(client_fd);
						continue;
					}

					// Create session
					ClientSession *sess = client_session_new(client_fd);
					if (!sess) {
						close(client_fd);
						continue;
					}

					// Add to session manager
					if (session_manager_add(mgr, sess) < 0) {
						fprintf(stderr, "Failed to add session\n");
						close(client_fd);
						client_session_free(sess);
						continue;
					}

					printf("New client connected (fd=%d, total=%d)\n", 
					       client_fd, session_manager_count(mgr));
				}
			} else {
				// Data from client - get session from epoll_event.data.ptr
				ClientSession *sess = (ClientSession *)events[i].data.ptr;
				if (!sess) continue;

				// Read packet (handles partial reads)
				uint16_t cmd;
				char *payload = NULL;
				uint32_t payload_len = 0;

				int result = client_session_read_packet(sess, &cmd, &payload, &payload_len);

				if (result < 0) {
					// Error or disconnect
					printf("Client disconnected (fd=%d, user_id=%ld)\n", fd, sess ? sess->user_id : 0);
					
					if (sess && sess->user_id > 0) {
						int64_t user_id = sess->user_id;
						int64_t room_id = sess->room_id;
						
						// [RECONNECT] Check if player is in a game and eligible for reconnect
						int reconnect_eligible = 0;
						if (sess->status == USER_STATUS_IN_GAME && room_id > 0) {
							// Player is in a game - save state for potential reconnect
							printf("[RECONNECT] Player disconnected during game, saving state for reconnect\n");
							printf("[RECONNECT] user_id=%ld, room_id=%ld\n", user_id, room_id);
							
							// Save pending reconnect state (this creates a 30s timer)
							PendingReconnect *pending = reconnect_save_state(
								user_id, room_id, sess->access_token, RECONNECT_GAME_MODE_1VN);
							
							if (pending) {
								reconnect_eligible = 1;  // Mark as reconnect eligible
								
								// Mark socket as closed but KEEP session in manager
								sess->socket_fd = -1;
								
								// Mark player as disconnected (not eliminated yet)
								onevn_mark_player_disconnected(room_id, user_id);
								
								// Broadcast to room that player is temporarily disconnected
								char notify_buf[256];
								snprintf(notify_buf, sizeof(notify_buf),
									"{\"user_id\": %ld, \"event\": \"disconnected\", \"reconnect_timeout\": 30}",
									user_id);
								session_manager_broadcast_to_room(room_id, 0x0411, // CMD_NOTIFY_ROOM_UPDATE
									notify_buf, strlen(notify_buf));
								
								printf("[RECONNECT] Session kept for reconnect, socket_fd=-1\n");
								fflush(stdout);
								
								// DO NOT deactivate session or notify friends offline yet
								// The reconnect timer callback will handle that if player doesn't reconnect
							} else {
								// Failed to save state, fall back to normal cleanup
								printf("[RECONNECT] Failed to save state, doing normal cleanup\n");
								dao_sessions_deactivate_all_by_user(user_id);
								quickmode_cleanup_user(user_id);
								friends_notify_status_change(user_id, "offline", 0);
							}
						} else {
							// Not in a game - normal disconnect cleanup
							printf("[AUTH] Normal disconnect (not in game), cleaning up\n");
							
							// [FORBID-LOGIN] Deactivate database session on disconnect
							extern int dao_sessions_deactivate_all_by_user(int64_t user_id);
							int deactivated = dao_sessions_deactivate_all_by_user(user_id);
							printf("[AUTH] Deactivated %d session(s) for user_id=%ld\n", 
							       deactivated, user_id);
							
							// Cleanup quickmode session if exists
							quickmode_cleanup_user(user_id);
							
							// Notify friends offline
							extern void friends_notify_status_change(int64_t user_id, const char *status, int64_t room_id);
							friends_notify_status_change(user_id, "offline", 0);
							printf("[FRIENDS] Notified: user_id=%ld is offline\n", user_id);
						}
						
						// Only remove session if NOT waiting for reconnect
						if (!reconnect_eligible) {
							session_manager_remove(mgr, fd);
						}
					} else {
						session_manager_remove(mgr, fd);
					}
					continue;
				}

				if (result == 0) {
					// Partial packet, wait for more data
					continue;
				}

				// Complete packet received
				dispatcher_handle_packet(sess, cmd, payload, payload_len);

				if (payload) free(payload);
			}
		}
	}

	printf("Shutting down server...\n");
	session_manager_free(mgr);
	close(sockfd);
	return 0;
}

