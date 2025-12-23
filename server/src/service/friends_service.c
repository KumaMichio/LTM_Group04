#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "service/friends_service.h"
#include "service/commands.h"
#include "service/protocol.h"
#include "service/session_manager.h"
#include "dao/dao_friends.h"
#include "dao/dao_users.h"
#include "dao/dao_rooms.h"
#include "dao/dao_chat.h"
#include "utils/json.h"

// Helper: Get online status string for a user
const char *friends_get_user_status(int64_t user_id) {
    ClientSession *sess = session_manager_get_by_user_id(user_id);
    if (!sess) return "offline";
    
    if (sess->room_id > 0) {
        // Check if room is in game
        // For simplicity, we'll check if room status is IN_PROGRESS
        // This would require a dao_rooms_get_status call, but for now we'll use "in_game" if in room
        return "in_game";
    }
    return "online";
}

// Helper: Broadcast friend status change to all friends of a user
static void broadcast_status_to_friends(int64_t user_id, const char *status, int64_t room_id) {
    void *friends_json = NULL;
    if (dao_friends_list(user_id, &friends_json) != 0) {
        return; // No friends or error
    }
    
    // Parse friends list
    int64_t friend_ids[256];
    int count = util_json_parse_user_id_array((const char *)friends_json, friend_ids, 256);
    free(friends_json);
    
    // Build status notification JSON
    char status_json[512];
    snprintf(status_json, sizeof(status_json),
        "{\"user_id\": %ld, \"status\": \"%s\", \"room_id\": %ld}",
        user_id, status, room_id);
    
    // Send to each friend
    for (int i = 0; i < count; i++) {
        session_manager_send_to_user(friend_ids[i], CMD_NOTIFY_FRIEND_STATUS,
            status_json, (uint32_t)strlen(status_json));
    }
}

// Notify friends when a user's status changes
void friends_notify_status_change(int64_t user_id, const char *status, int64_t room_id) {
    broadcast_status_to_friends(user_id, status, room_id);
}

// Handle search user
static void handle_search_user(ClientSession *sess, const char *payload) {
    char *query = util_json_get_string(payload, "query");
    if (!query) {
        protocol_send_error(sess, CMD_RES_SEARCH_USER, "MISSING_QUERY");
        return;
    }
    
    int limit = 20;
    long long limit_val_ll = 0;
    if (util_json_get_int64(payload, "limit", &limit_val_ll) && limit_val_ll > 0) {
        limit = (int)limit_val_ll;
        if (limit > 100) limit = 100; // Cap at 100
    }
    
    void *result_json = NULL;
    if (dao_users_search_by_username(query, limit, &result_json) != 0) {
        free(query);
        protocol_send_error(sess, CMD_RES_SEARCH_USER, "SEARCH_FAILED");
        return;
    }
    
    // Free query after use
    free(query);
    
    const char *json_str = (const char *)result_json;
    protocol_send_response(sess, CMD_RES_SEARCH_USER, json_str, strlen(json_str));
    free(result_json);
}

// Handle get friend info with online status
static void handle_get_friend_info(ClientSession *sess, const char *payload) {
    long long friend_id_ll = 0;
    if (!util_json_get_int64(payload, "friend_id", &friend_id_ll) || friend_id_ll <= 0) {
        protocol_send_error(sess, CMD_RES_GET_FRIEND_INFO, "INVALID_FRIEND_ID");
        return;
    }
    int64_t friend_id = (int64_t)friend_id_ll;
    
    // Get friend info from DB
    void *info_json = NULL;
    if (dao_friends_get_info(sess->user_id, friend_id, &info_json) != 0) {
        protocol_send_error(sess, CMD_RES_GET_FRIEND_INFO, "USER_NOT_FOUND");
        return;
    }
    
    // Parse the JSON to add online status
    char *info_str = (char *)info_json;
    size_t info_len = strlen(info_str);
    
    // Check online status
    const char *status = friends_get_user_status(friend_id);
    ClientSession *friend_sess = session_manager_get_by_user_id(friend_id);
    int64_t room_id = friend_sess ? friend_sess->room_id : 0;
    
    // Build enhanced JSON with online status
    char enhanced_json[2048];
    // Remove closing brace, add status fields
    if (info_len > 0 && info_str[info_len - 1] == '}') {
        info_str[info_len - 1] = '\0';
    }
    snprintf(enhanced_json, sizeof(enhanced_json),
        "%s, \"online_status\": \"%s\", \"room_id\": %ld}",
        info_str, status, room_id);
    
    protocol_send_response(sess, CMD_RES_GET_FRIEND_INFO, enhanced_json, strlen(enhanced_json));
    free(info_json);
}

// Handle add friend request
static void handle_add_friend(ClientSession *sess, const char *payload) {
    long long friend_id_ll = 0;
    if (!util_json_get_int64(payload, "friend_id", &friend_id_ll) || friend_id_ll <= 0) {
        protocol_send_error(sess, CMD_RES_ADD_FRIEND, "INVALID_FRIEND_ID");
        return;
    }
    int64_t friend_id = (int64_t)friend_id_ll;
    
    // Check if trying to add self
    if (friend_id == sess->user_id) {
        protocol_send_error(sess, CMD_RES_ADD_FRIEND, "CANNOT_ADD_SELF");
        return;
    }
    
    // Send friend request
    if (dao_friends_send_request(sess->user_id, friend_id) != 0) {
        protocol_send_error(sess, CMD_RES_ADD_FRIEND, "ADD_FRIEND_FAILED");
        return;
    }
    
    // Send notification to friend if online
    ClientSession *friend_sess = session_manager_get_by_user_id(friend_id);
    if (friend_sess) {
        User sender;
        if (dao_users_find_by_id(sess->user_id, &sender) == 0) {
            char notify_json[512];
            char *esc_username = util_json_escape(sender.username);
            if (!esc_username) esc_username = strdup("");
            
            snprintf(notify_json, sizeof(notify_json),
                "{\"from_user_id\": %lld, \"from_username\": \"%s\"}",
                (long long)sess->user_id, esc_username);
            
            session_manager_send_to_user(friend_id, CMD_NOTIFY_FRIEND_REQ,
                notify_json, (uint32_t)strlen(notify_json));
            
            free(esc_username);
        }
    }
    
    // Send success response
    protocol_send_simple_ok(sess, CMD_RES_ADD_FRIEND);
}

// Handle list friends
static void handle_list_friends(ClientSession *sess) {
    void *result_json = NULL;
    if (dao_friends_list(sess->user_id, &result_json) != 0) {
        protocol_send_error(sess, CMD_RES_LIST_FRIENDS, "LIST_FRIENDS_FAILED");
        return;
    }
    
    const char *json_str = (const char *)result_json;
    protocol_send_response(sess, CMD_RES_LIST_FRIENDS, json_str, strlen(json_str));
    free(result_json);
}

// Handle get pending friend requests
static void handle_get_pending_requests(ClientSession *sess) {
    void *result_json = NULL;
    if (dao_friends_get_pending_requests(sess->user_id, &result_json) != 0) {
        protocol_send_error(sess, CMD_RES_GET_PENDING_REQ, "GET_PENDING_FAILED");
        return;
    }
    
    const char *json_str = (const char *)result_json;
    protocol_send_response(sess, CMD_RES_GET_PENDING_REQ, json_str, strlen(json_str));
    free(result_json);
}

// Handle invite friend to room
static void handle_invite_friend(ClientSession *sess, const char *payload) {
    long long friend_id_ll = 0;
    long long room_id_ll = 0;
    
    if (!util_json_get_int64(payload, "friend_id", &friend_id_ll) || friend_id_ll <= 0) {
        protocol_send_error(sess, CMD_RES_INVITE_FRIEND, "INVALID_FRIEND_ID");
        return;
    }
    int64_t friend_id = (int64_t)friend_id_ll;
    
    // Get room_id from session or payload
    int64_t room_id = 0;
    if (util_json_get_int64(payload, "room_id", &room_id_ll) && room_id_ll > 0) {
        room_id = (int64_t)room_id_ll;
    } else if (sess->room_id > 0) {
        room_id = sess->room_id;
    } else {
        protocol_send_error(sess, CMD_RES_INVITE_FRIEND, "NOT_IN_ROOM");
        return;
    }
    
    // Check if they are friends
    bool are_friends = false;
    if (dao_friends_are_friends(sess->user_id, friend_id, &are_friends) != 0 || !are_friends) {
        protocol_send_error(sess, CMD_RES_INVITE_FRIEND, "NOT_FRIENDS");
        return;
    }
    
    // Check if friend is online
    ClientSession *friend_sess = session_manager_get_by_user_id(friend_id);
    if (!friend_sess) {
        protocol_send_error(sess, CMD_RES_INVITE_FRIEND, "FRIEND_OFFLINE");
        return;
    }
    
    // Get room info
    int64_t owner_id = 0;
    if (dao_rooms_get_owner(room_id, &owner_id) != 0) {
        protocol_send_error(sess, CMD_RES_INVITE_FRIEND, "ROOM_NOT_FOUND");
        return;
    }
    
    // Get sender username
    User sender;
    if (dao_users_find_by_id(sess->user_id, &sender) != 0) {
        protocol_send_error(sess, CMD_RES_INVITE_FRIEND, "USER_NOT_FOUND");
        return;
    }
    
    // Send invite notification to friend
    char invite_json[512];
    char *esc_username = util_json_escape(sender.username);
    if (!esc_username) esc_username = strdup("");
    
    snprintf(invite_json, sizeof(invite_json),
        "{\"from_user_id\": %ld, \"from_username\": \"%s\", \"room_id\": %ld}",
        sess->user_id, esc_username, room_id);
    
    session_manager_send_to_user(friend_id, CMD_NOTIFY_ROOM_INVITE,
        invite_json, (uint32_t)strlen(invite_json));
    
    free(esc_username);
    
    // Send success response to sender
    protocol_send_simple_ok(sess, CMD_RES_INVITE_FRIEND);
}

// Handle send DM
static void handle_send_dm(ClientSession *sess, const char *payload) {
    long long to_user_id_ll = 0;
    
    if (!util_json_get_int64(payload, "to_user_id", &to_user_id_ll) || to_user_id_ll <= 0) {
        protocol_send_error(sess, CMD_RES_SEND_DM, "INVALID_TO_USER_ID");
        return;
    }
    int64_t to_user_id = (int64_t)to_user_id_ll;
    
    char *message = util_json_get_string(payload, "message");
    if (!message) {
        protocol_send_error(sess, CMD_RES_SEND_DM, "MISSING_MESSAGE");
        return;
    }
    
    // Check if they are friends (optional, can be removed if DMs are open)
    bool are_friends = false;
    dao_friends_are_friends(sess->user_id, to_user_id, &are_friends);
    // For now, we allow DMs even if not friends
    
    // Check if user is logged in
    if (sess->user_id <= 0) {
        protocol_send_error(sess, CMD_RES_SEND_DM, "NOT_LOGGED_IN");
        return;
    }
    
    // Get sender info
    User sender;
    if (dao_users_find_by_id(sess->user_id, &sender) != 0) {
        fprintf(stderr, "[FRIENDS] SENDER_NOT_FOUND: user_id=%lld\n", (long long)sess->user_id);
        protocol_send_error(sess, CMD_RES_SEND_DM, "SENDER_NOT_FOUND");
        return;
    }
    
    // Build DM notification JSON
    char *esc_username = util_json_escape(sender.username);
    char *esc_message = util_json_escape(message);
    if (!esc_username) esc_username = strdup("");
    if (!esc_message) esc_message = strdup("");
    
    // Save message to database
    if (dao_chat_send_dm(sess->user_id, to_user_id, message) != 0) {
        free(esc_username);
        free(esc_message);
        free(message);
        protocol_send_error(sess, CMD_RES_SEND_DM, "SAVE_MESSAGE_FAILED");
        return;
    }
    
    char dm_json[2048];
    snprintf(dm_json, sizeof(dm_json),
        "{\"from_user_id\": %lld, \"from_username\": \"%s\", \"message\": \"%s\", \"timestamp\": %ld}",
        (long long)sess->user_id, esc_username, esc_message, (long)time(NULL));
    
    // Try to send to user if online
    int sent = session_manager_send_to_user(to_user_id, CMD_NOTIFY_DM,
        dm_json, (uint32_t)strlen(dm_json));
    
    // Always return success if saved to DB, even if user is offline
    protocol_send_simple_ok(sess, CMD_RES_SEND_DM);
    
    free(esc_username);
    free(esc_message);
    free(message);
}

// Handle send room chat
static void handle_send_room_chat(ClientSession *sess, const char *payload) {
    char *message = util_json_get_string(payload, "message");
    if (!message) {
        protocol_send_error(sess, CMD_RES_SEND_ROOM_CHAT, "MISSING_MESSAGE");
        return;
    }
    
    // Get room_id from session or payload
    int64_t room_id = 0;
    long long room_id_ll = 0;
    if (util_json_get_int64(payload, "room_id", &room_id_ll) && room_id_ll > 0) {
        room_id = (int64_t)room_id_ll;
    } else if (sess->room_id > 0) {
        room_id = sess->room_id;
    } else {
        free(message);
        protocol_send_error(sess, CMD_RES_SEND_ROOM_CHAT, "NOT_IN_ROOM");
        return;
    }
    
    // Check if user is logged in
    if (sess->user_id <= 0) {
        free(message);
        protocol_send_error(sess, CMD_RES_SEND_ROOM_CHAT, "NOT_LOGGED_IN");
        return;
    }
    
    // Get sender info
    User sender;
    if (dao_users_find_by_id(sess->user_id, &sender) != 0) {
        free(message);
        fprintf(stderr, "[FRIENDS] SENDER_NOT_FOUND: user_id=%lld\n", (long long)sess->user_id);
        protocol_send_error(sess, CMD_RES_SEND_ROOM_CHAT, "SENDER_NOT_FOUND");
        return;
    }
    
    // Save message to database
    if (dao_chat_send_room(sess->user_id, room_id, message) != 0) {
        free(message);
        protocol_send_error(sess, CMD_RES_SEND_ROOM_CHAT, "SAVE_MESSAGE_FAILED");
        return;
    }
    
    // Build room chat message JSON
    char *esc_username = util_json_escape(sender.username);
    char *esc_message = util_json_escape(message);
    if (!esc_username) esc_username = strdup("");
    if (!esc_message) esc_message = strdup("");
    
    char chat_json[2048];
    snprintf(chat_json, sizeof(chat_json),
        "{\"user_id\": %lld, \"username\": \"%s\", \"message\": \"%s\", \"timestamp\": %ld}",
        (long long)sess->user_id, esc_username, esc_message, (long)time(NULL));
    
    // Broadcast to all room members
    int sent = session_manager_broadcast_to_room(room_id, CMD_NOTIFY_ROOM_CHAT,
        chat_json, (uint32_t)strlen(chat_json));
    
    // Always return success if saved to DB
    protocol_send_simple_ok(sess, CMD_RES_SEND_ROOM_CHAT);
    
    free(esc_username);
    free(esc_message);
    free(message);
}

// Handle fetch offline messages
static void handle_fetch_offline(ClientSession *sess) {
    void *json_result = NULL;
    
    if (dao_chat_fetch_offline(sess->user_id, &json_result) != 0) {
        protocol_send_error(sess, CMD_RES_FETCH_OFFLINE, "FETCH_OFFLINE_FAILED");
        return;
    }
    
    if (json_result) {
        char *json_str = (char *)json_result;
        protocol_send_response(sess, CMD_RES_FETCH_OFFLINE, json_str, (uint32_t)strlen(json_str));
        free(json_result);
    } else {
        // No offline messages
        protocol_send_response(sess, CMD_RES_FETCH_OFFLINE, "[]", 2);
    }
}

// Main dispatch function
void friends_dispatch(ClientSession *sess, uint16_t cmd, const char *payload, uint32_t payload_len) {
    (void)payload_len; // Unused for now
    
    switch (cmd) {
        case CMD_REQ_SEARCH_USER:
            handle_search_user(sess, payload);
            break;
            
        case CMD_REQ_ADD_FRIEND:
            handle_add_friend(sess, payload);
            break;
            
        case CMD_REQ_LIST_FRIENDS:
            handle_list_friends(sess);
            break;
            
        case CMD_REQ_GET_FRIEND_INFO:
            handle_get_friend_info(sess, payload);
            break;
            
        case CMD_REQ_GET_PENDING_REQ:
            handle_get_pending_requests(sess);
            break;
            
        case CMD_REQ_INVITE_FRIEND:
            handle_invite_friend(sess, payload);
            break;
            
        case CMD_REQ_SEND_DM:
            handle_send_dm(sess, payload);
            break;
            
        case CMD_REQ_SEND_ROOM_CHAT:
            handle_send_room_chat(sess, payload);
            break;
            
        case CMD_REQ_FETCH_OFFLINE:
            handle_fetch_offline(sess);
            break;
            
        default:
            protocol_send_error(sess, cmd, "UNKNOWN_FRIENDS_CMD");
            break;
    }
}

