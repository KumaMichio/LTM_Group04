// TCP server with non-blocking I/O using select()
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

#include "service/server.h"
#include "service/client_session.h"
#include "service/dispatcher.h"
#include "service/protocol.h"
#include "service/session_manager.h"
#include "service/friends_service.h"
#include "utils/timer.h"

static volatile int running = 1;

static void handle_sigint(int signum) {
	(void)signum;
	running = 0;
}

// Set socket to non-blocking mode
static int set_nonblocking(int fd) {
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Process read buffer and extract complete packets
static void process_client_buffer(ClientSession *sess) {
	if (!sess || sess->read_buffer_len < sizeof(PacketHeader)) {
		return;
	}

	while (sess->read_buffer_len >= sizeof(PacketHeader)) {
		PacketHeader *hdr = (PacketHeader *)sess->read_buffer;
		uint16_t cmd = ntohs(hdr->cmd);
		uint32_t len = ntohl(hdr->length);

		// Check if we have complete packet
		size_t packet_size = sizeof(PacketHeader) + len;
		if (sess->read_buffer_len < packet_size) {
			// Wait for more data
			break;
		}

		// Extract payload
		char *payload = NULL;
		if (len > 0) {
			payload = malloc(len + 1);
			if (!payload) {
				// Out of memory, skip this packet
				memmove(sess->read_buffer, 
				        sess->read_buffer + packet_size,
				        sess->read_buffer_len - packet_size);
				sess->read_buffer_len -= packet_size;
				continue;
			}
			memcpy(payload, sess->read_buffer + sizeof(PacketHeader), len);
			payload[len] = '\0';
		}

		// Process packet
		dispatcher_handle_packet(sess, cmd, payload, len);

		if (payload) free(payload);

		// Remove processed packet from buffer
		if (sess->read_buffer_len > packet_size) {
			memmove(sess->read_buffer,
			        sess->read_buffer + packet_size,
			        sess->read_buffer_len - packet_size);
		}
		sess->read_buffer_len -= packet_size;
	}
}

// Handle data from a client socket
static void handle_client_read(ClientSession *sess) {
	if (!sess || sess->socket_fd < 0) return;

	// Read available data
	size_t available = CLIENT_BUFFER_SIZE - sess->read_buffer_len;
	if (available == 0) {
		// Buffer full, close connection
		fprintf(stderr, "Client buffer full, closing connection\n");
		if (sess->user_id > 0) {
			friends_notify_status_change(sess->user_id, "offline", 0);
		}
		close(sess->socket_fd);
		sess->socket_fd = -1;
		return;
	}

	ssize_t n = recv(sess->socket_fd, 
	                 sess->read_buffer + sess->read_buffer_len,
	                 available, 0);

	if (n < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			// No data available, this is normal for non-blocking
			return;
		}
		// Error reading, close connection
		fprintf(stderr, "Error reading from client: %s\n", strerror(errno));
		if (sess->user_id > 0) {
			friends_notify_status_change(sess->user_id, "offline", 0);
		}
		close(sess->socket_fd);
		sess->socket_fd = -1;
		return;
	}

	if (n == 0) {
		// Client closed connection
		if (sess->user_id > 0) {
			friends_notify_status_change(sess->user_id, "offline", 0);
		}
		close(sess->socket_fd);
		sess->socket_fd = -1;
		return;
	}

	// Update buffer length
	sess->read_buffer_len += n;

	// Process complete packets from buffer
	process_client_buffer(sess);
}

// Clean up disconnected sessions
static void cleanup_disconnected_sessions(void) {
	ClientSession *current = session_manager_get_all_sessions();
	ClientSession *next;

	while (current) {
		next = current->next;
		if (current->socket_fd < 0) {
			// Session disconnected
			if (current->user_id > 0) {
				friends_notify_status_change(current->user_id, "offline", 0);
			}
			session_manager_unregister(current);
			client_session_free(current);
		}
		current = next;
	}
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

	// Set listening socket to non-blocking
	if (set_nonblocking(sockfd) < 0) {
		fprintf(stderr, "Failed to set listening socket non-blocking\n");
		close(sockfd);
		return -1;
	}

	if (listen(sockfd, 16) != 0) {
		fprintf(stderr, "listen failed: %s\n", strerror(errno));
		close(sockfd);
		return -1;
	}

	signal(SIGINT, handle_sigint);

	printf("Server listening on %s:%s (non-blocking I/O with select)\n", 
	       bind_addr ? bind_addr : "0.0.0.0", portstr);

	while (running) {
		fd_set read_fds;
		int max_fd = sockfd;
		struct timeval timeout;

		FD_ZERO(&read_fds);
		FD_SET(sockfd, &read_fds);

		// Add all client sockets to select set
		ClientSession *sess = session_manager_get_all_sessions();
		while (sess) {
			if (sess->socket_fd >= 0) {
				FD_SET(sess->socket_fd, &read_fds);
				if (sess->socket_fd > max_fd) {
					max_fd = sess->socket_fd;
				}
			}
			sess = sess->next;
		}

		// Set timeout to 1 second for timer cleanup
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;

		int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

		if (activity < 0) {
			if (errno == EINTR) continue;
			fprintf(stderr, "select error: %s\n", strerror(errno));
			continue;
		}

		if (activity == 0) {
			// Timeout - cleanup timers and disconnected sessions
			timer_cleanup();
			cleanup_disconnected_sessions();
			continue;
		}

		// Check for new connections
		if (FD_ISSET(sockfd, &read_fds)) {
			struct sockaddr_storage cli_addr;
			socklen_t cli_len = sizeof(cli_addr);
			int fd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
			
			if (fd >= 0) {
				// Set client socket to non-blocking
				if (set_nonblocking(fd) < 0) {
					fprintf(stderr, "Failed to set client socket non-blocking\n");
					close(fd);
					continue;
				}

				ClientSession *sess = client_session_new(fd);
				if (!sess) {
					close(fd);
					continue;
				}

				// Register session
				session_manager_register(sess);
			} else {
				if (errno != EAGAIN && errno != EWOULDBLOCK) {
					fprintf(stderr, "accept error: %s\n", strerror(errno));
				}
			}
		}

		// Check for data from clients
		sess = session_manager_get_all_sessions();
		while (sess) {
			ClientSession *next = sess->next;
			if (sess->socket_fd >= 0 && FD_ISSET(sess->socket_fd, &read_fds)) {
				handle_client_read(sess);
			}
			sess = next;
		}

		// Periodic cleanup
		timer_cleanup();
		cleanup_disconnected_sessions();
	}

	// Cleanup: close all client connections
	ClientSession *sess = session_manager_get_all_sessions();
	while (sess) {
		ClientSession *next = sess->next;
		if (sess->socket_fd >= 0) {
			close(sess->socket_fd);
		}
		if (sess->user_id > 0) {
			friends_notify_status_change(sess->user_id, "offline", 0);
		}
		session_manager_unregister(sess);
		client_session_free(sess);
		sess = next;
	}

	close(sockfd);
	return 0;
}
