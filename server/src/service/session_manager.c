// server/src/service/session_manager.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "service/session_manager.h"
#include "service/protocol.h"

// Simple linked list of active sessions
static ClientSession *active_sessions = NULL;
static int session_count = 0;

void session_manager_register(ClientSession *sess) {
    if (!sess) return;
    
    // Add to linked list
    sess->next = active_sessions;
    active_sessions = sess;
    session_count++;
}

void session_manager_unregister(ClientSession *sess) {
    if (!sess) return;
    
    // Remove from linked list
    if (active_sessions == sess) {
        active_sessions = sess->next;
        sess->next = NULL;
        session_count--;
        return;
    }
    
    ClientSession *prev = active_sessions;
    while (prev && prev->next != sess) {
        prev = prev->next;
    }
    
    if (prev) {
        prev->next = sess->next;
        sess->next = NULL;
        session_count--;
    }
}

void session_manager_set_room(ClientSession *sess, int64_t room_id) {
    if (!sess) return;
    sess->room_id = room_id;
}

int session_manager_get_room_sessions(int64_t room_id, ClientSession **out_sessions, int max_count) {
    if (!out_sessions || max_count <= 0) return 0;
    
    int count = 0;
    ClientSession *current = active_sessions;
    
    while (current && count < max_count) {
        if (current->room_id == room_id && current->user_id > 0) {
            out_sessions[count++] = current;
        }
        current = current->next;
    }
    
    return count;
}

ClientSession *session_manager_get_by_user_id(int64_t user_id) {
    if (user_id <= 0) return NULL;
    
    ClientSession *current = active_sessions;
    while (current) {
        if (current->user_id == user_id) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

int session_manager_broadcast_to_room(int64_t room_id, uint16_t cmd, const char *json, uint32_t json_len) {
    if (room_id <= 0 || !json) return 0;
    
    ClientSession *sessions[32]; // Max 32 sessions per room
    int count = session_manager_get_room_sessions(room_id, sessions, 32);
    
    int success_count = 0;
    for (int i = 0; i < count; i++) {
        if (sessions[i] && sessions[i]->socket_fd >= 0) {
            protocol_send_response(sessions[i], cmd, json, json_len);
            success_count++;
        }
    }
    
    return success_count;
}

int session_manager_send_to_user(int64_t user_id, uint16_t cmd, const char *json, uint32_t json_len) {
    if (user_id <= 0 || !json) return 0;
    
    ClientSession *sess = session_manager_get_by_user_id(user_id);
    if (sess && sess->socket_fd >= 0) {
        protocol_send_response(sess, cmd, json, json_len);
        return 1;
    }
    
    return 0;
}

ClientSession *session_manager_get_all_sessions(void) {
    return active_sessions;
}

