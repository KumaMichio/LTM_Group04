// server/include/utils/timer.h
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

typedef struct {
    time_t start_time;
    int duration_seconds;
    int64_t context_id;  // session_id or room_id
    void (*callback)(int64_t context_id, void *user_data);
    void *user_data;
} GameTimer;

// Create a timer (non-blocking, needs to be checked manually)
// Returns timer_id (or -1 on error)
int game_timer_create(int duration_seconds, int64_t context_id, 
                     void (*callback)(int64_t context_id, void *user_data),
                     void *user_data);

// Check if timer has expired (call this periodically)
// Returns 1 if expired, 0 if still running, -1 if not found
int timer_check_expired(int timer_id);

// Get remaining seconds for a timer
// Returns remaining seconds or -1 if not found/expired
int timer_get_remaining(int timer_id);

// Cancel a timer
void timer_cancel(int timer_id);

// Cleanup expired timers
void timer_cleanup(void);

#endif
