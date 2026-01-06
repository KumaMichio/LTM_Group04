// server/include/service/reconnect_manager.h
// Manages pending reconnect states for players who disconnect during a game
#ifndef RECONNECT_MANAGER_H
#define RECONNECT_MANAGER_H

#include <stdint.h>
#include <time.h>
#include "service/client_session.h"

// Grace period in seconds for allowing reconnect
#define RECONNECT_GRACE_PERIOD 30

// Game mode types
typedef enum {
    RECONNECT_GAME_MODE_NONE = 0,
    RECONNECT_GAME_MODE_QUICKMODE = 1,
    RECONNECT_GAME_MODE_1VN = 2
} ReconnectGameMode;

// Pending reconnect state
typedef struct PendingReconnect {
    int64_t user_id;
    int64_t room_id;
    char access_token[65];
    time_t disconnect_time;
    
    // Game mode
    ReconnectGameMode game_mode;
    
    // 1vN specific state
    int64_t session_id;       // 1vN game session ID
    int player_index;         // Index in game state arrays
    int total_score;          // Score at disconnect time
    int consecutive_correct;  // Combo at disconnect time
    int current_round;        // Round number at disconnect
    
    // Timer for grace period
    int timer_id;
    
    struct PendingReconnect *next;
} PendingReconnect;

/**
 * Save pending reconnect state when player disconnects during a game
 * Returns: pointer to created state, or NULL on failure
 */
PendingReconnect *reconnect_save_state(int64_t user_id, int64_t room_id, 
                                        const char *access_token, 
                                        ReconnectGameMode game_mode);

/**
 * Find pending reconnect state by user_id
 * Returns: pointer to state if found, NULL otherwise
 */
PendingReconnect *reconnect_find_pending(int64_t user_id);

/**
 * Remove pending reconnect state by user_id
 * Returns: 0 on success, -1 if not found
 */
int reconnect_remove_pending(int64_t user_id);

/**
 * Handle reconnect request from client
 * This validates token, checks grace period, and restores session
 */
void reconnect_handle_request(ClientSession *sess, const char *payload, uint32_t payload_len);

/**
 * Callback when grace period expires
 * This will eliminate the player from the game
 */
void reconnect_timeout_callback(int64_t context_id, void *user_data);

/**
 * Cleanup all expired pending states
 */
void reconnect_cleanup_expired(void);

/**
 * Get count of pending reconnects (for debugging)
 */
int reconnect_get_pending_count(void);

#endif
