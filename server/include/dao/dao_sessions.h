// server/include/dao/dao_sessions.h
#ifndef DAO_SESSIONS_H
#define DAO_SESSIONS_H

#include <stdint.h>
#include <time.h>

typedef struct {
    int64_t id;
    int64_t user_id;
    char    access_token[65]; // CHAR(64) + '\0'
    time_t  expires_at;
} UserSession;

// tạo session mới cho user, gen token 64 hex char
int dao_sessions_create(int64_t user_id, int ttl_seconds, UserSession *out_sess);

// lấy theo token, 0=OK, -1=không thấy
int dao_sessions_find_by_token(const char *token, UserSession *out_sess);

// cập nhật last_heartbeat + expires_at (refresh)
int dao_sessions_touch(const char *token, int ttl_seconds);

// [FORBID-LOGIN] Kiểm tra user có active session không
// Returns: 0 = no active session, 1 = has active session, -1 = db error
int dao_sessions_count_active_by_user(int64_t user_id);

// [FORBID-LOGIN] Deactivate tất cả active sessions của user
// Returns: 0 = success, -1 = db error
int dao_sessions_deactivate_all_by_user(int64_t user_id);

// [FORBID-LOGIN] Cleanup all sessions on server restart
// Returns: number of sessions cleaned up, -1 = db error
int dao_sessions_cleanup_all_on_restart();

// [HEARTBEAT] Cleanup stale sessions (last_heartbeat > stale_seconds)
// Returns: number of sessions cleaned up, -1 = db error
int dao_sessions_cleanup_stale(int stale_seconds);

// [HEARTBEAT] Find stale user IDs (last_heartbeat > stale_seconds)
// Returns: number of stale users found, -1 = db error
// out_user_ids: array of user_ids (caller must free)
int dao_sessions_find_stale_users(int stale_seconds, int64_t **out_user_ids);

#endif
