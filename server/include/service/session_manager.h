// Session manager for handling multiple client connections
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include "service/client_session.h"
#include <sys/epoll.h>

#define MAX_EPOLL_EVENTS 64
#define MAX_SESSIONS 1024

typedef struct SessionManager {
	ClientSession **sessions;  // Array of session pointers
	int max_sessions;
	int session_count;
	int epoll_fd;
} SessionManager;

// Create/destroy session manager
SessionManager *session_manager_new(int max_sessions);
void session_manager_free(SessionManager *mgr);

// Add/remove sessions
int session_manager_add(SessionManager *mgr, ClientSession *sess);
int session_manager_remove(SessionManager *mgr, int socket_fd);
ClientSession *session_manager_get_by_fd(SessionManager *mgr, int socket_fd);

// Epoll management
int session_manager_epoll_add(SessionManager *mgr, int fd, uint32_t events);
int session_manager_epoll_modify(SessionManager *mgr, int fd, uint32_t events);
int session_manager_epoll_remove(SessionManager *mgr, int fd);

// Get epoll fd for waiting
int session_manager_get_epoll_fd(SessionManager *mgr);

// Get count
int session_manager_count(SessionManager *mgr);

#endif
