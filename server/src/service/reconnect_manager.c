// server/src/service/reconnect_manager.c
// Implementation of reconnect state management for players who disconnect during a game

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "service/reconnect_manager.h"
#include "service/onevn_service.h"
#include "service/session_manager.h"
#include "service/protocol.h"
#include "service/commands.h"
#include "service/friends_service.h"
#include "dao/dao_sessions.h"
#include "dao/dao_rooms.h"
#include "utils/timer.h"
#include "utils/json.h"

// Linked list of pending reconnect states
static PendingReconnect *pending_list = NULL;
static int pending_count = 0;

// Forward declaration
static void free_pending(PendingReconnect *pending);

PendingReconnect *reconnect_save_state(int64_t user_id, int64_t room_id, 
                                        const char *access_token, 
                                        ReconnectGameMode game_mode) {
    if (user_id <= 0 || !access_token) {
        return NULL;
    }
    
    // Check if already exists
    PendingReconnect *existing = reconnect_find_pending(user_id);
    if (existing) {
        printf("[RECONNECT] Pending state already exists for user_id=%ld, updating\n", (long)user_id);
        // Update existing state
        existing->disconnect_time = time(NULL);
        existing->room_id = room_id;
        existing->game_mode = game_mode;
        return existing;
    }
    
    // Create new pending state
    PendingReconnect *pending = calloc(1, sizeof(PendingReconnect));
    if (!pending) {
        printf("[RECONNECT] ERROR: Failed to allocate PendingReconnect\n");
        return NULL;
    }
    
    pending->user_id = user_id;
    pending->room_id = room_id;
    strncpy(pending->access_token, access_token, sizeof(pending->access_token) - 1);
    pending->disconnect_time = time(NULL);
    pending->game_mode = game_mode;
    pending->timer_id = -1;
    
    // Get game state snapshot if in 1vN mode
    if (game_mode == RECONNECT_GAME_MODE_1VN && room_id > 0) {
        int score = 0, consecutive = 0, current_round = 0;
        if (onevn_get_player_state(room_id, user_id, &score, &consecutive, &current_round) == 0) {
            pending->total_score = score;
            pending->consecutive_correct = consecutive;
            pending->current_round = current_round;
            printf("[RECONNECT] Saved 1vN state: score=%d, consecutive=%d, round=%d\n",
                   score, consecutive, current_round);
        }
    }
    
    // Create timer for grace period
    pending->timer_id = game_timer_create(RECONNECT_GRACE_PERIOD, user_id, 
                                           reconnect_timeout_callback, pending);
    if (pending->timer_id < 0) {
        printf("[RECONNECT] WARNING: Failed to create grace period timer\n");
    } else {
        printf("[RECONNECT] Created grace period timer id=%d for %d seconds\n", 
               pending->timer_id, RECONNECT_GRACE_PERIOD);
    }
    
    // Add to list
    pending->next = pending_list;
    pending_list = pending;
    pending_count++;
    
    printf("[RECONNECT] Saved pending state for user_id=%ld, room_id=%ld, mode=%d (total pending=%d)\n",
           (long)user_id, (long)room_id, game_mode, pending_count);
    fflush(stdout);
    
    return pending;
}

PendingReconnect *reconnect_find_pending(int64_t user_id) {
    PendingReconnect *p = pending_list;
    while (p) {
        if (p->user_id == user_id) {
            return p;
        }
        p = p->next;
    }
    return NULL;
}

int reconnect_remove_pending(int64_t user_id) {
    PendingReconnect **pp = &pending_list;
    while (*pp) {
        if ((*pp)->user_id == user_id) {
            PendingReconnect *to_remove = *pp;
            *pp = to_remove->next;
            
            // Cancel timer if active
            if (to_remove->timer_id >= 0) {
                game_timer_cancel(to_remove->timer_id);
            }
            
            free_pending(to_remove);
            pending_count--;
            printf("[RECONNECT] Removed pending state for user_id=%ld (total pending=%d)\n",
                   (long)user_id, pending_count);
            return 0;
        }
        pp = &(*pp)->next;
    }
    return -1;
}

static void free_pending(PendingReconnect *pending) {
    if (pending) {
        free(pending);
    }
}

void reconnect_timeout_callback(int64_t context_id, void *user_data) {
    int64_t user_id = context_id;
    PendingReconnect *pending = (PendingReconnect *)user_data;
    
    // Validate that pending still exists and matches
    PendingReconnect *found = reconnect_find_pending(user_id);
    if (!found || found != pending) {
        printf("[RECONNECT] Timeout callback: pending state already removed for user_id=%ld\n",
               (long)user_id);
        return;
    }
    
    printf("[RECONNECT] ========== GRACE PERIOD EXPIRED for user_id=%ld ==========\n", (long)user_id);
    fflush(stdout);
    
    // Now actually eliminate from game
    if (pending->game_mode == RECONNECT_GAME_MODE_1VN && pending->room_id > 0) {
        // Mark as eliminated in game state
        int result = onevn_eliminate_player_by_room(pending->room_id, user_id);
        printf("[RECONNECT] onevn_eliminate_player_by_room returned: %d\n", result);
        
        // Also mark in database
        dao_rooms_mark_eliminated(pending->room_id, user_id);
        
        // Broadcast elimination to other players
        char elim_buf[256];
        snprintf(elim_buf, sizeof(elim_buf),
            "{\"user_id\": %ld, \"reason\": \"disconnect_timeout\"}", (long)user_id);
        session_manager_broadcast_to_room(pending->room_id, CMD_NOTIFY_ELIMINATION,
                                          elim_buf, strlen(elim_buf));
        printf("[RECONNECT] Broadcast elimination for user_id=%ld\n", (long)user_id);
    }
    
    // Deactivate database session
    dao_sessions_deactivate_all_by_user(user_id);
    
    // Notify friends that user is offline
    friends_notify_status_change(user_id, "offline", 0);
    
    // Remove pending state (don't cancel timer since we're in the callback)
    pending->timer_id = -1;  // Prevent double cancel
    reconnect_remove_pending(user_id);
    
    printf("[RECONNECT] Cleanup complete for user_id=%ld\n", (long)user_id);
    fflush(stdout);
}

void reconnect_handle_request(ClientSession *sess, const char *payload, uint32_t payload_len) {
    (void)payload_len;
    
    if (!sess || !payload) {
        printf("[RECONNECT] Invalid session or payload\n");
        return;
    }
    
    // Parse payload
    int64_t user_id = 0;
    char *access_token = NULL;
    
    util_json_get_int64(payload, "user_id", &user_id);
    access_token = util_json_get_string(payload, "access_token");
    
    printf("[RECONNECT] Reconnect request: user_id=%ld, token=%s\n",
           (long)user_id, access_token ? access_token : "(null)");
    fflush(stdout);
    
    if (user_id <= 0 || !access_token || strlen(access_token) == 0) {
        protocol_send_error(sess, CMD_RES_RECONNECT, "INVALID_PAYLOAD");
        free(access_token);
        return;
    }
    
    // Find pending reconnect state
    PendingReconnect *pending = reconnect_find_pending(user_id);
    if (!pending) {
        printf("[RECONNECT] No pending state found for user_id=%ld\n", (long)user_id);
        protocol_send_error(sess, CMD_RES_RECONNECT, "NO_PENDING_SESSION");
        free(access_token);
        return;
    }
    
    // Verify access token
    if (strcmp(pending->access_token, access_token) != 0) {
        printf("[RECONNECT] Token mismatch for user_id=%ld\n", (long)user_id);
        protocol_send_error(sess, CMD_RES_RECONNECT, "INVALID_TOKEN");
        free(access_token);
        return;
    }
    
    // Check grace period
    time_t now = time(NULL);
    int elapsed = (int)(now - pending->disconnect_time);
    if (elapsed > RECONNECT_GRACE_PERIOD) {
        printf("[RECONNECT] Grace period expired for user_id=%ld (elapsed=%d seconds)\n",
               (long)user_id, elapsed);
        protocol_send_error(sess, CMD_RES_RECONNECT, "GRACE_PERIOD_EXPIRED");
        reconnect_remove_pending(user_id);
        free(access_token);
        return;
    }
    
    // SUCCESS - Restore session
    printf("[RECONNECT] Restoring session for user_id=%ld (elapsed=%d seconds)\n",
           (long)user_id, elapsed);
    
    sess->user_id = pending->user_id;
    sess->room_id = pending->room_id;
    strncpy(sess->access_token, pending->access_token, sizeof(sess->access_token) - 1);
    sess->status = USER_STATUS_IN_GAME;
    
    // Restore in game engine
    int restore_result = -1;
    int time_remaining = 0;
    int score = pending->total_score;
    int consecutive = pending->consecutive_correct;
    int current_round = pending->current_round;
    
    if (pending->game_mode == RECONNECT_GAME_MODE_1VN) {
        restore_result = onevn_restore_player(pending->room_id, user_id);
        
        if (restore_result == 0) {
            // Get current game state including time remaining
            onevn_get_player_state(pending->room_id, user_id, &score, &consecutive, &current_round);
            time_remaining = onevn_get_round_time_remaining(pending->room_id);
        }
    }
    
    if (restore_result == 0) {
        // Build success response with game state
        char response[1024];
        snprintf(response, sizeof(response),
            "{\"status\": \"reconnected\", \"room_id\": %ld, \"score\": %d, "
            "\"consecutive_correct\": %d, \"current_round\": %d, \"time_remaining\": %d}",
            (long)pending->room_id, score, consecutive, current_round, time_remaining);
        protocol_send_response(sess, CMD_RES_RECONNECT, response, strlen(response));
        
        printf("[RECONNECT] Sent reconnect success response: %s\n", response);
        
        // Broadcast player reconnected to room
        char notify[256];
        snprintf(notify, sizeof(notify), 
            "{\"user_id\": %ld, \"event\": \"reconnected\"}", (long)user_id);
        session_manager_broadcast_to_room(pending->room_id, CMD_NOTIFY_ROOM_UPDATE,
                                          notify, strlen(notify));
        
        // Update session manager status
        session_manager_update_status(user_id, USER_STATUS_IN_GAME, pending->room_id);
        
        // Remove pending state (this also cancels the timeout timer)
        reconnect_remove_pending(user_id);
        
        printf("[RECONNECT] Player user_id=%ld successfully reconnected to room_id=%ld\n",
               (long)user_id, (long)pending->room_id);
    } else if (restore_result == -2) {
        printf("[RECONNECT] Player already eliminated for user_id=%ld\n", (long)user_id);
        protocol_send_error(sess, CMD_RES_RECONNECT, "ALREADY_ELIMINATED");
        reconnect_remove_pending(user_id);
    } else {
        printf("[RECONNECT] Game not found for user_id=%ld\n", (long)user_id);
        protocol_send_error(sess, CMD_RES_RECONNECT, "GAME_NOT_FOUND");
        reconnect_remove_pending(user_id);
    }
    
    free(access_token);
    fflush(stdout);
}

void reconnect_cleanup_expired(void) {
    time_t now = time(NULL);
    PendingReconnect **pp = &pending_list;
    
    while (*pp) {
        int elapsed = (int)(now - (*pp)->disconnect_time);
        if (elapsed > RECONNECT_GRACE_PERIOD + 10) {  // Extra 10 seconds buffer
            PendingReconnect *expired = *pp;
            *pp = expired->next;
            
            printf("[RECONNECT] Cleaning up expired pending state for user_id=%ld\n",
                   (long)expired->user_id);
            
            if (expired->timer_id >= 0) {
                game_timer_cancel(expired->timer_id);
            }
            free_pending(expired);
            pending_count--;
        } else {
            pp = &(*pp)->next;
        }
    }
}

int reconnect_get_pending_count(void) {
    return pending_count;
}
