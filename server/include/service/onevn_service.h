// server/include/service/onevn_service.h
#ifndef ONEVN_SERVICE_H
#define ONEVN_SERVICE_H

#include <stdint.h>
#include "service/client_session.h"

// Dispatch 1vN-related commands (0x06xx)
void onevn_dispatch(ClientSession *sess, uint16_t cmd, const char *payload, uint32_t payload_len);

// Mark a player as eliminated in the active game by room_id
// This updates both in-memory game state and database
// Returns 0 if success, -1 if game not found or player not in game
int onevn_eliminate_player_by_room(int64_t room_id, int64_t user_id);

// Mark a player as temporarily disconnected (awaiting reconnect)
// This does NOT eliminate them - they can still reconnect within grace period
// Returns: 0 = success, -1 = not found, -2 = already eliminated
int onevn_mark_player_disconnected(int64_t room_id, int64_t user_id);

// Restore a player who has reconnected
// Returns: 0 = success, -1 = not found, -2 = already eliminated
int onevn_restore_player(int64_t room_id, int64_t user_id);

// Get player's current game state snapshot for reconnect
// Returns: 0 = success, -1 = not found
int onevn_get_player_state(int64_t room_id, int64_t user_id, 
                           int *out_score, int *out_consecutive, int *out_current_round);

// Get remaining time in current round (for reconnect scenario)
// Returns: seconds remaining, or 0 if no active round
int onevn_get_round_time_remaining(int64_t room_id);

#endif

