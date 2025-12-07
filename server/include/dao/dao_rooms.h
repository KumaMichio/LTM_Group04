#ifndef DAO_ROOMS_H
#define DAO_ROOMS_H

#include <stdint.h>

typedef enum {
    ROOM_STATUS_WAITING,
    ROOM_STATUS_STARTING,
    ROOM_STATUS_IN_PROGRESS,
    ROOM_STATUS_FINISHED
} room_status_t;

int dao_rooms_create(int64_t owner_id, int64_t *out_room_id);  // Removed mode parameter
int dao_rooms_join(int64_t room_id, int64_t user_id, int is_owner);
int dao_rooms_leave(int64_t room_id, int64_t user_id);
int dao_rooms_get_members(int64_t room_id, void **result_json);
int dao_rooms_update_status(int64_t room_id, room_status_t status);

#endif
