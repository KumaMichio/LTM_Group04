#include "service/dispatcher.h"
#include "service/commands.h"
#include "service/auth_service.h"
#include "service/quickmode_service.h"
#include "service/stats_service.h"
#include "service/session_manager.h"
#include "service/onevn_service.h"
#include "service/friends_service.h"
// Nếu tách riêng friends/chat/room:
#include "dao/dao_friends.h"
#include "dao/dao_chat.h"
#include "dao/dao_rooms.h"
#include "dao/dao_users.h"
#include "service/protocol.h"
#include "utils/json.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>




void dispatcher_handle_packet(ClientSession *sess, uint16_t cmd, const char *payload, uint32_t payload_len) {
    uint8_t major = (cmd & 0xFF00) >> 8;
    // uint8_t minor = cmd & 0x00FF;

    switch (major) {
        case 0x01: // Auth
            auth_dispatch(sess, cmd, payload, payload_len);
            break;

        case 0x02: // Friends
            switch (cmd) {
                case CMD_REQ_ADD_FRIEND: {
                    // parse payload: { "friend_id": 123 }
                    long long friend_id = 0;
                    util_json_get_int64(payload, "friend_id", &friend_id);
                    if (dao_friends_send_request(sess->user_id, friend_id) == 0) {
                        protocol_send_simple_ok(sess, CMD_RES_ADD_FRIEND);
                        
                        // Notify friend about incoming request
                        User sender;
                        if (dao_users_find_by_id(sess->user_id, &sender) == 0) {
                            char notify_json[512];
                            char *esc_username = util_json_escape(sender.username);
                            if (!esc_username) esc_username = strdup("");
                            snprintf(notify_json, sizeof(notify_json),
                                "{\"from_user_id\": %ld, \"from_username\": \"%s\"}",
                                sess->user_id, esc_username);
                            session_manager_send_to_user(friend_id, CMD_NOTIFY_FRIEND_REQ,
                                notify_json, (uint32_t)strlen(notify_json));
                            free(esc_username);
                        }
                    } else {
                        protocol_send_error(sess, CMD_RES_ADD_FRIEND, "ADD_FRIEND_FAILED");
                    }
                } break;
                case CMD_REQ_RESPOND_FRIEND: {
                    long long from_user = 0;
                    long long accept_ll = 0;
                    util_json_get_int64(payload, "from_user", &from_user);
                    util_json_get_int64(payload, "accept", &accept_ll);
                    int accept = accept_ll ? 1 : 0;
                    if (dao_friends_respond_request(from_user, sess->user_id, accept) == 0) {
                        protocol_send_simple_ok(sess, CMD_RES_RESPOND_FRIEND);
                        
                        // If accepted, notify both users about friend status
                        if (accept) {
                            const char *status = "online";
                            int64_t room_id = 0;
                            ClientSession *from_sess = session_manager_get_by_user_id(from_user);
                            if (from_sess) {
                                room_id = from_sess->room_id;
                                if (room_id > 0) status = "in_game";
                            } else {
                                status = "offline";
                            }
                            
                            // Notify sender that friend accepted
                            friends_notify_status_change(sess->user_id, status, room_id);
                            // Notify new friend about sender's status
                            friends_notify_status_change(from_user, 
                                friends_get_user_status(sess->user_id), sess->room_id);
                        }
                    } else {
                        protocol_send_error(sess, CMD_RES_RESPOND_FRIEND, "RESPOND_FRIEND_FAILED");
                    }
                } break;
                case CMD_REQ_LIST_FRIENDS: {
                    void *json_list = NULL;
                    if (dao_friends_list(sess->user_id, &json_list) != 0) {
                        protocol_send_error(sess, CMD_RES_LIST_FRIENDS, "LIST_FRIENDS_FAILED");
                    } else {
                        const char *json_str = (const char *)json_list;
                        protocol_send_response(sess, CMD_RES_LIST_FRIENDS, json_str, strlen(json_str));
                        free(json_list);
                    }
                } break;
                case CMD_REQ_SEARCH_USER:
                case CMD_REQ_GET_FRIEND_INFO:
                case CMD_REQ_GET_PENDING_REQ:
                    friends_dispatch(sess, cmd, payload, payload_len);
                    break;
                default:
                    protocol_send_error(sess, cmd, "UNKNOWN_FRIENDS_CMD");
            }
            break;

        case 0x03: // Chat
            switch (cmd) {
                case CMD_REQ_SEND_DM:
                    friends_dispatch(sess, cmd, payload, payload_len);
                    break;
                case CMD_REQ_SEND_ROOM_CHAT: {
                    long long room_id = 0;
                    util_json_get_int64(payload, "room_id", &room_id);
                    char *msg = util_json_get_string(payload, "content");
                    if (dao_chat_send_room(sess->user_id, room_id, msg) == 0) {
                        protocol_send_simple_ok(sess, CMD_RES_SEND_ROOM_CHAT);
                        // broadcast NOTIFY_ROOM_CHAT cho các member trong room
                    } else {
                        protocol_send_error(sess, CMD_RES_SEND_ROOM_CHAT, "SEND_ROOM_CHAT_FAILED");
                    }
                    if (msg) free(msg);
                } break;
                case CMD_REQ_FETCH_OFFLINE: {
                    void *json_msgs = NULL;
                    if (dao_chat_fetch_offline(sess->user_id, &json_msgs) != 0) {
                        protocol_send_error(sess, CMD_RES_FETCH_OFFLINE, "FETCH_OFFLINE_FAILED");
                    } else {
                        const char *json_str = (const char *)json_msgs;
                        protocol_send_response(sess, CMD_RES_FETCH_OFFLINE, json_str, strlen(json_str));
                        dao_chat_mark_read(sess->user_id);
                        free(json_msgs);
                    }
                } break;
                default:
                    protocol_send_error(sess, cmd, "UNKNOWN_CHAT_CMD");
            }
            break;

        case 0x04: // Room
            switch (cmd) {
                case CMD_REQ_CREATE_ROOM: {
                    // Parse config if provided
                    int easy_count = 10, medium_count = 10, hard_count = 10;  // defaults
                    long long easy_ll = 0, medium_ll = 0, hard_ll = 0;
                    if (util_json_get_int64(payload, "easy_count", &easy_ll)) {
                        easy_count = (int)easy_ll;
                    }
                    if (util_json_get_int64(payload, "medium_count", &medium_ll)) {
                        medium_count = (int)medium_ll;
                    }
                    if (util_json_get_int64(payload, "hard_count", &hard_ll)) {
                        hard_count = (int)hard_ll;
                    }

                    // Validate total
                    if (easy_count + medium_count + hard_count > 30) {
                        protocol_send_error(sess, CMD_RES_CREATE_ROOM, "TOO_MANY_QUESTIONS");
                        break;
                    }

                    int64_t room_id;
                    if (dao_rooms_create_with_config(sess->user_id, easy_count, medium_count, hard_count, &room_id) == 0) {
                        // trả về info room
                        char buf[256];
                        snprintf(buf, sizeof(buf), 
                            "{\"room_id\": %ld, \"easy_count\": %d, \"medium_count\": %d, \"hard_count\": %d}", 
                            room_id, easy_count, medium_count, hard_count);
                        protocol_send_response(sess, CMD_RES_CREATE_ROOM, buf, strlen(buf));
                    } else {
                        protocol_send_error(sess, CMD_RES_CREATE_ROOM, "CREATE_ROOM_FAILED");
                    }
                } break;
                case CMD_REQ_JOIN_ROOM: {
                    long long room_id_ll = 0; // parse
                    util_json_get_int64(payload, "room_id", &room_id_ll);
                    int64_t room_id = (int64_t)room_id_ll;
                    if (dao_rooms_join(room_id, sess->user_id, 0) == 0) {
                        // Update session's room_id
                        session_manager_set_room(sess, room_id);
                        protocol_send_simple_ok(sess, CMD_RES_JOIN_ROOM);
                        
                        // Notify friends that user joined a room (status = in_game)
                        friends_notify_status_change(sess->user_id, "in_game", room_id);
                        
                        // broadcast NOTIFY_ROOM_UPDATE cho các member
                        void *members_json = NULL;
                        if (dao_rooms_get_members(room_id, &members_json) == 0) {
                            char notify_json[512];
                            snprintf(notify_json, sizeof(notify_json),
                                "{\"room_id\":%lld,\"members\":%s}",
                                (long long)room_id, (const char *)members_json);
                            session_manager_broadcast_to_room(room_id, CMD_NOTIFY_ROOM_UPDATE,
                                                             notify_json, strlen(notify_json));
                            free(members_json);
                        }
                    } else {
                        protocol_send_error(sess, CMD_RES_JOIN_ROOM, "JOIN_ROOM_FAILED");
                    }
                } break;
                case CMD_REQ_LEAVE_ROOM: {
                    long long room_id_ll = 0;
                    util_json_get_int64(payload, "room_id", &room_id_ll);
                    int64_t room_id = (int64_t)room_id_ll;
                    if (room_id == 0 && sess->room_id > 0) {
                        room_id = sess->room_id;
                    }
                    if (dao_rooms_leave(room_id, sess->user_id) == 0) {
                        session_manager_set_room(sess, 0);
                        protocol_send_simple_ok(sess, CMD_RES_LEAVE_ROOM);
                        
                        // Notify friends that user left room (status = online)
                        friends_notify_status_change(sess->user_id, "online", 0);
                        
                        // broadcast NOTIFY_ROOM_UPDATE
                        void *members_json = NULL;
                        if (dao_rooms_get_members(room_id, &members_json) == 0) {
                            char notify_json[512];
                            snprintf(notify_json, sizeof(notify_json),
                                "{\"room_id\":%lld,\"members\":%s}",
                                (long long)room_id, (const char *)members_json);
                            session_manager_broadcast_to_room(room_id, CMD_NOTIFY_ROOM_UPDATE,
                                                             notify_json, strlen(notify_json));
                            free(members_json);
                        }
                    } else {
                        protocol_send_error(sess, CMD_RES_LEAVE_ROOM, "LEAVE_ROOM_FAILED");
                    }
                } break;
                case CMD_REQ_INVITE_FRIEND:
                    friends_dispatch(sess, cmd, payload, payload_len);
                    break;
                case CMD_REQ_START_GAME: {
                    // Start 1vN game - handled by onevn_service
                    onevn_dispatch(sess, cmd, payload, payload_len);
                } break;
                default:
                    protocol_send_error(sess, cmd, "UNKNOWN_ROOM_CMD");
            }
            break;

        case 0x05: // Basic Mode – Quickmode
            quickmode_dispatch(sess, cmd, payload, payload_len);
            break;

        case 0x06: // 1vN MODE
            onevn_dispatch(sess, cmd, payload, payload_len);
            break;

        case 0x07: // Stats
            switch (cmd) {
                case CMD_REQ_GET_PROFILE:
                    stats_handle_get_profile(sess, cmd, payload, payload_len);
                    break;
                case CMD_REQ_LEADERBOARD:
                    stats_handle_leaderboard(sess, cmd, payload, payload_len);
                    break;
                case CMD_REQ_MATCH_HISTORY:
                    stats_handle_match_history(sess, cmd, payload, payload_len);
                    break;
                default:
                    protocol_send_error(sess, cmd, "UNKNOWN_STATS_CMD");
            }
            break;

        default:
            protocol_send_error(sess, cmd, "UNKNOWN_CMD");
    }
}
