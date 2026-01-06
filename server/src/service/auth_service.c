// server/src/service/auth_service.c
#include <stdio.h>
#include "dao/dao_users.h"
#include "dao/dao_sessions.h"
#include "service/auth_service.h"
#include "service/commands.h"
#include "service/protocol.h"
#include "service/friends_service.h"
#include "service/session_manager.h"
#include "service/quickmode_service.h"
#include "utils/json.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#define SESSION_TTL_SECONDS 3600

AuthResult auth_signup(const char *username, const char *password) {
    int64_t user_id;
    int rc = dao_users_create(username, password, &user_id);
    if (rc == -2) {
        return AUTH_ERR_EXIST;
    }
    if (rc != 0) {
        return AUTH_ERR_DB;
    }
    printf("[AUTH] Signup OK: user_id=%lld\n", (long long)user_id);
    return AUTH_OK;
}

AuthResult auth_login(const char *username, const char *password,
                      UserSession *out_session) {
    int64_t user_id = 0;
    int rc = dao_users_check_password(username, password, &user_id);
    if (rc < 0) {
        return AUTH_ERR_DB;
    }
    if (rc == 0) {
        return AUTH_ERR_CRED;
    }

    // [FORBID-LOGIN] Check if user already has active session
    int active_count = dao_sessions_count_active_by_user(user_id);
    if (active_count < 0) {
        // DB error
        return AUTH_ERR_DB;
    }
    if (active_count > 0) {
        // User already logged in from another device
        printf("[AUTH] Login rejected: User %lld already has active session(s)\n", (long long)user_id);
        return AUTH_ERR_USER_ALREADY_LOGGED_IN;
    }

    if (dao_sessions_create(user_id, SESSION_TTL_SECONDS, out_session) != 0) {
        return AUTH_ERR_DB;
    }

    printf("[AUTH] Login OK: user_id=%lld, token=%s\n",
           (long long)user_id, out_session->access_token);
    return AUTH_OK;
}

// Very small dispatcher implementation.
void auth_dispatch(ClientSession *sess, uint16_t cmd, const char *payload, uint32_t payload_len) {
    (void)payload_len;
    switch (cmd) {
        case CMD_REQ_REGISTER: {
            char *username = util_json_get_string(payload, "username");
            char *password = util_json_get_string(payload, "password");
            if (!username || !password) {
                protocol_send_error(sess, CMD_RES_REGISTER, "INVALID_PAYLOAD");
            } else {
                AuthResult r = auth_signup(username, password);
                if (r == AUTH_OK) protocol_send_simple_ok(sess, CMD_RES_REGISTER);
                else if (r == AUTH_ERR_EXIST) protocol_send_error(sess, CMD_RES_REGISTER, "USERNAME_EXISTS");
                else protocol_send_error(sess, CMD_RES_REGISTER, "REGISTER_FAILED");
            }
            free(username); free(password);
        } break;

        case CMD_REQ_LOGIN: {
            char *username = util_json_get_string(payload, "username");
            char *password = util_json_get_string(payload, "password");
            if (!username || !password) {
                protocol_send_error(sess, CMD_RES_LOGIN, "INVALID_PAYLOAD");
            } else {
                UserSession us;
                AuthResult r = auth_login(username, password, &us);
                if (r == AUTH_OK) {
                    // attach to session
                    sess->user_id = us.user_id;
                    strncpy(sess->access_token, us.access_token, sizeof(sess->access_token)-1);
                    
                    // Update status to ONLINE
                    session_manager_update_status(us.user_id, USER_STATUS_ONLINE, 0);
                    
                    // reply with token and user_id
                    char buf[256];
                    int n = snprintf(buf, sizeof(buf), 
                        "{\"token\": \"%s\", \"user_id\": %lld}", 
                        us.access_token, (long long)us.user_id);
                    protocol_send_response(sess, CMD_RES_LOGIN, buf, (uint32_t)n);
                    
                    // Notify friends that this user is now online
                    // Note: This must be called AFTER setting sess->user_id and sending response
                    // to ensure session is properly registered
                    printf("[AUTH] Notifying friends that user_id=%lld is now online\n", 
                           (long long)us.user_id);
                    fflush(stdout);
                    friends_notify_status_change(us.user_id, "online", 0);
                } else {
                    // [FORBID-LOGIN] Handle login errors
                    if (r == AUTH_ERR_USER_ALREADY_LOGGED_IN) {
                        protocol_send_error(sess, CMD_RES_LOGIN, 
                            "USER_ALREADY_LOGGED_IN");
                        printf("[AUTH] Login rejected: User already logged in from another device\n");
                    } else if (r == AUTH_ERR_CRED) {
                        protocol_send_error(sess, CMD_RES_LOGIN, 
                            "INVALID_CREDENTIALS");
                        printf("[AUTH] Login failed: Invalid credentials\n");
                    } else {
                        protocol_send_error(sess, CMD_RES_LOGIN, "LOGIN_FAILED");
                    }
                }
            }
            free(username); free(password);
        } break;

        case CMD_REQ_LOGOUT: {
            if (sess && sess->user_id > 0) {
                int64_t user_id = sess->user_id;
                
                // [FORBID-LOGIN] Deactivate session in database
                printf("[AUTH] Deactivating session for user %lld\n", (long long)user_id);
                int deactivate_result = dao_sessions_deactivate_all_by_user(user_id);
                if (deactivate_result != 0) {
                    printf("[WARN] Failed to deactivate session in database for user %lld\n", (long long)user_id);
                }
                
                // Update status to offline
                session_manager_update_status(user_id, USER_STATUS_OFFLINE, 0);
                
                // Cleanup game sessions (QuickMode)
                quickmode_cleanup_user(user_id);
                
                // Notify friends that user is offline
                friends_notify_status_change(user_id, "offline", 0);
                
                // Clear session data
                sess->user_id = 0;
                sess->access_token[0] = '\0';
                sess->room_id = 0;
                
                // Send logout success response
                protocol_send_simple_ok(sess, CMD_RES_LOGOUT);
                
                printf("[AUTH] User %lld logged out successfully\n", (long long)user_id);
                fflush(stdout);
            } else {
                protocol_send_error(sess, CMD_RES_LOGOUT, "NOT_LOGGED_IN");
            }
        } break;

        default:
            protocol_send_error(sess, cmd, "UNKNOWN_AUTH_CMD");
    }
}
