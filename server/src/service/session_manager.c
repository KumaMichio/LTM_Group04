// Session manager implementation
#include "service/session_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/epoll.h>

SessionManager *session_manager_new(int max_sessions) {
	SessionManager *mgr = calloc(1, sizeof(SessionManager));
	if (!mgr) return NULL;

	mgr->max_sessions = max_sessions;
	mgr->sessions = calloc(max_sessions, sizeof(ClientSession*));
	
	if (!mgr->sessions) {
		free(mgr->sessions);
		free(mgr);
		return NULL;
	}

	// Create epoll instance
	mgr->epoll_fd = epoll_create1(0);
	if (mgr->epoll_fd < 0) {
		free(mgr->sessions);
		free(mgr);
		return NULL;
	}

	mgr->session_count = 0;
	return mgr;
}

void session_manager_free(SessionManager *mgr) {
	if (!mgr) return;

	// Close all sessions
	for (int i = 0; i < mgr->max_sessions; i++) {
		if (mgr->sessions[i]) {
			if (mgr->sessions[i]->socket_fd >= 0) {
				close(mgr->sessions[i]->socket_fd);
			}
			client_session_free(mgr->sessions[i]);
		}
	}

	if (mgr->epoll_fd >= 0) {
		close(mgr->epoll_fd);
	}

	free(mgr->sessions);
	free(mgr);
}

int session_manager_add(SessionManager *mgr, ClientSession *sess) {
	if (!mgr || !sess || sess->socket_fd < 0) return -1;
	if (mgr->session_count >= mgr->max_sessions) return -1;

	// Find free slot
	int index = -1;
	for (int i = 0; i < mgr->max_sessions; i++) {
		if (mgr->sessions[i] == NULL) {
			index = i;
			break;
		}
	}

	if (index < 0) return -1;

	// Add to epoll - store session pointer in epoll_event.data.ptr
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET; // Edge-triggered, read events
	ev.data.ptr = sess; // Store session pointer directly

	if (epoll_ctl(mgr->epoll_fd, EPOLL_CTL_ADD, sess->socket_fd, &ev) < 0) {
		return -1;
	}

	// Store session
	mgr->sessions[index] = sess;
	mgr->session_count++;

	return 0;
}

int session_manager_remove(SessionManager *mgr, int socket_fd) {
	if (!mgr || socket_fd < 0) return -1;

	// Find session by socket_fd
	ClientSession *sess = NULL;
	int index = -1;
	for (int i = 0; i < mgr->max_sessions; i++) {
		if (mgr->sessions[i] && mgr->sessions[i]->socket_fd == socket_fd) {
			sess = mgr->sessions[i];
			index = i;
			break;
		}
	}

	if (!sess || index < 0) return -1;

	// Remove from epoll
	epoll_ctl(mgr->epoll_fd, EPOLL_CTL_DEL, socket_fd, NULL);

	// Free session
	client_session_free(sess);
	mgr->sessions[index] = NULL;
	mgr->session_count--;

	return 0;
}

ClientSession *session_manager_get_by_fd(SessionManager *mgr, int socket_fd) {
	if (!mgr || socket_fd < 0) return NULL;

	// Find session by socket_fd
	for (int i = 0; i < mgr->max_sessions; i++) {
		if (mgr->sessions[i] && mgr->sessions[i]->socket_fd == socket_fd) {
			return mgr->sessions[i];
		}
	}

	return NULL;
}

int session_manager_epoll_add(SessionManager *mgr, int fd, uint32_t events) {
	if (!mgr || fd < 0) return -1;

	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd; // For server socket, we use fd

	return epoll_ctl(mgr->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
}

int session_manager_epoll_modify(SessionManager *mgr, int fd, uint32_t events) {
	if (!mgr || fd < 0) return -1;

	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;

	return epoll_ctl(mgr->epoll_fd, EPOLL_CTL_MOD, fd, &ev);
}

int session_manager_epoll_remove(SessionManager *mgr, int fd) {
	if (!mgr || fd < 0) return -1;
	return epoll_ctl(mgr->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

int session_manager_get_epoll_fd(SessionManager *mgr) {
	return mgr ? mgr->epoll_fd : -1;
}

int session_manager_count(SessionManager *mgr) {
	return mgr ? mgr->session_count : 0;
}
