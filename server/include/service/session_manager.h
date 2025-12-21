// server/include/service/session_manager.h
#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <stdint.h>
#include "service/client_session.h"

// Session manager for tracking active connections and room memberships

// Register a session (call when client connects)
void session_manager_register(ClientSession *sess);

// Unregister a session (call when client disconnects)
void session_manager_unregister(ClientSession *sess);

// Update session's room_id
void session_manager_set_room(ClientSession *sess, int64_t room_id);

// Get all sessions in a room
// Returns number of sessions found, stores pointers in out_sessions array (max max_count)
int session_manager_get_room_sessions(int64_t room_id, ClientSession **out_sessions, int max_count);

// Get session by user_id
ClientSession *session_manager_get_by_user_id(int64_t user_id);

// Broadcast message to all sessions in a room
int session_manager_broadcast_to_room(int64_t room_id, uint16_t cmd, const char *json, uint32_t json_len);

// Broadcast message to a specific user (if online)
int session_manager_send_to_user(int64_t user_id, uint16_t cmd, const char *json, uint32_t json_len);

// Get all active sessions (for iteration, returns head of linked list)
ClientSession *session_manager_get_all_sessions(void);

#endif

