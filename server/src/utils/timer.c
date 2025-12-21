// server/src/utils/timer.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utils/timer.h"

#define MAX_TIMERS 100

static GameTimer timers[MAX_TIMERS];
static int timer_count = 0;
static int next_timer_id = 1;

int game_timer_create(int duration_seconds, int64_t context_id,
                      void (*callback)(int64_t context_id, void *user_data),
                      void *user_data) {
    if (timer_count >= MAX_TIMERS) return -1;
    
    int timer_id = next_timer_id++;
    if (next_timer_id < 0) next_timer_id = 1; // Wrap around
    
    // Find empty slot
    int slot = -1;
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].duration_seconds == 0) {
            slot = i;
            break;
        }
    }
    
    if (slot < 0) return -1;
    
    timers[slot].start_time = time(NULL);
    timers[slot].duration_seconds = duration_seconds;
    timers[slot].context_id = context_id;
    timers[slot].callback = callback;
    timers[slot].user_data = user_data;
    
    timer_count++;
    return timer_id;
}

int timer_check_expired(int timer_id) {
    time_t now = time(NULL);
    
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].duration_seconds > 0) {
            // Simple check: if timer_id matches or we check all
            time_t elapsed = now - timers[i].start_time;
            if (elapsed >= timers[i].duration_seconds) {
                // Timer expired
                if (timers[i].callback) {
                    timers[i].callback(timers[i].context_id, timers[i].user_data);
                }
                
                // Clear timer
                memset(&timers[i], 0, sizeof(GameTimer));
                timer_count--;
                return 1;
            }
        }
    }
    
    return 0;
}

int timer_get_remaining(int timer_id) {
    time_t now = time(NULL);
    
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].duration_seconds > 0) {
            time_t elapsed = now - timers[i].start_time;
            int remaining = timers[i].duration_seconds - (int)elapsed;
            return remaining > 0 ? remaining : 0;
        }
    }
    
    return -1;
}

void timer_cancel(int timer_id) {
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].duration_seconds > 0) {
            memset(&timers[i], 0, sizeof(GameTimer));
            timer_count--;
            return;
        }
    }
}

void timer_cleanup(void) {
    time_t now = time(NULL);
    
    for (int i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].duration_seconds > 0) {
            time_t elapsed = now - timers[i].start_time;
            if (elapsed >= timers[i].duration_seconds) {
                // Expired, clear it
                if (timers[i].callback) {
                    timers[i].callback(timers[i].context_id, timers[i].user_data);
                }
                memset(&timers[i], 0, sizeof(GameTimer));
                timer_count--;
            }
        }
    }
}
