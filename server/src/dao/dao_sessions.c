// server/src/dao/dao_sessions.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "db.h"
#include <openssl/rand.h>
#include "dao/dao_sessions.h"

// sinh chuỗi hex random 64 ký tự
static void gen_token(char *buf, size_t len) {
    // Expect len >= 65 (64 hex chars + '\0')
    if (len < 65) return;

    unsigned char rnd[32];
    if (RAND_bytes(rnd, sizeof(rnd)) == 1) {
        static const char hex[] = "0123456789abcdef";
        for (size_t i = 0; i < sizeof(rnd); ++i) {
            buf[i*2]   = hex[(rnd[i] >> 4) & 0xF];
            buf[i*2+1] = hex[rnd[i] & 0xF];
        }
        buf[64] = '\0';
        return;
    }

    // fallback to rand() if RAND_bytes fails
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < len - 1; ++i) {
        unsigned v = rand() % 16;
        buf[i] = hex[v];
    }
    buf[len - 1] = '\0';
}

int dao_sessions_create(int64_t user_id, int ttl_seconds, UserSession *out_sess) {
    if (!db_is_ok()) return -1;

    const char *sql =
        "INSERT INTO user_sessions (user_id, access_token, expires_at) "
        "VALUES ($1, $2, NOW() + ($3 || ' seconds')::interval) "
        "RETURNING id, user_id, access_token, EXTRACT(EPOCH FROM expires_at);";

    char user_id_str[32], ttl_str[32];
    snprintf(user_id_str, sizeof(user_id_str), "%lld", (long long)user_id);
    snprintf(ttl_str, sizeof(ttl_str), "%d", ttl_seconds);

    const char *params[3];


    // Try generating a token and inserting. On unique constraint violation (duplicate token)
    // retry a few times instead of failing immediately.
    const int MAX_TRIES = 8;
    PGresult *res = NULL;
    int try;
    for (try = 0; try < MAX_TRIES; ++try) {
        char token[65];
        gen_token(token, sizeof(token));
        params[0] = user_id_str;
        params[1] = token;
        params[2] = ttl_str;

        int paramLengthsLocal[3] = { (int)strlen(user_id_str), (int)strlen(token), (int)strlen(ttl_str) };
        int paramFormatsLocal[3] = { 0, 0, 0 };

        res = PQexecParams(db_conn,
                           sql,
                           3,
                           NULL,
                           params,
                           paramLengthsLocal,
                           paramFormatsLocal,
                           0);

        if (PQresultStatus(res) == PGRES_TUPLES_OK) {
            // success
            break;
        }

        // If unique constraint on access_token -> retry with a new token
        const char *sqlstate = PQresultErrorField(res, PG_DIAG_SQLSTATE);
        if (sqlstate && strcmp(sqlstate, "23505") == 0) {
            // duplicate key, try again
            PQclear(res);
            res = NULL;
            continue;
        }

        // other error -> log and return
        db_log_error(res, "dao_sessions_create failed");
        PQclear(res);
        return -1;
    }

    if (!res) {
        // exhausted tries
        fprintf(stderr, "dao_sessions_create: exhausted token generation attempts\n");
        return -1;
    }

    if (PQntuples(res) != 1) {
        db_log_error(res, "dao_sessions_create no row");
        PQclear(res);
        return -1;
    }

    if (out_sess) {
        memset(out_sess, 0, sizeof(*out_sess));
        out_sess->id       = atoll(PQgetvalue(res, 0, 0));
        out_sess->user_id  = atoll(PQgetvalue(res, 0, 1));
        strncpy(out_sess->access_token, PQgetvalue(res, 0, 2), 64);
        out_sess->expires_at = (time_t)atoll(PQgetvalue(res, 0, 3));
    }

    PQclear(res);
    return 0;
}

int dao_sessions_find_by_token(const char *token, UserSession *out_sess) {
    if (!db_is_ok()) return -1;

    const char *sql =
        "SELECT id, user_id, access_token, EXTRACT(EPOCH FROM expires_at) "
        "FROM user_sessions WHERE access_token = $1;";

    const char *params[1] = { token };
    int paramLengths[1]   = { (int)strlen(token) };
    int paramFormats[1]   = { 0 };

    PGresult *res = PQexecParams(db_conn,
                                 sql,
                                 1,
                                 NULL,
                                 params,
                                 paramLengths,
                                 paramFormats,
                                 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        db_log_error(res, "dao_sessions_find_by_token failed");
        return -1;
    }

    if (PQntuples(res) == 0) {
        PQclear(res);
        return -1;
    }

    if (out_sess) {
        memset(out_sess, 0, sizeof(*out_sess));
        out_sess->id       = atoll(PQgetvalue(res, 0, 0));
        out_sess->user_id  = atoll(PQgetvalue(res, 0, 1));
        strncpy(out_sess->access_token, PQgetvalue(res, 0, 2), 64);
        out_sess->expires_at = (time_t)atoll(PQgetvalue(res, 0, 3));
    }

    PQclear(res);
    return 0;
}

int dao_sessions_touch(const char *token, int ttl_seconds) {
    if (!db_is_ok()) return -1;

    const char *sql =
        "UPDATE user_sessions "
        "SET last_heartbeat = NOW(), "
        "    expires_at = NOW() + ($2 || ' seconds')::interval "
        "WHERE access_token = $1;";

    char ttl_str[32];
    snprintf(ttl_str, sizeof(ttl_str), "%d", ttl_seconds);

    const char *params[2] = { token, ttl_str };
    int paramLengths[2]   = { (int)strlen(token), (int)strlen(ttl_str) };
    int paramFormats[2]   = { 0,0 };

    PGresult *res = PQexecParams(db_conn,
                                 sql,
                                 2,
                                 NULL,
                                 params,
                                 paramLengths,
                                 paramFormats,
                                 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        db_log_error(res, "dao_sessions_touch failed");
        return -1;
    }

    int affected = atoi(PQcmdTuples(res));
    PQclear(res);
    return affected > 0 ? 0 : -1;
}

// [FORBID-LOGIN] Check if user has active session(s)
// Returns: 0 = no active session, 1 = has active session, -1 = db error
int dao_sessions_count_active_by_user(int64_t user_id) {
    if (!db_is_ok()) return -1;

    const char *sql =
        "SELECT COUNT(*) FROM user_sessions "
        "WHERE user_id = $1 AND expires_at > NOW();";

    char user_id_str[32];
    snprintf(user_id_str, sizeof(user_id_str), "%lld", (long long)user_id);

    const char *params[1] = { user_id_str };
    int paramLengths[1]   = { (int)strlen(user_id_str) };
    int paramFormats[1]   = { 0 };

    PGresult *res = PQexecParams(db_conn,
                                 sql,
                                 1,
                                 NULL,
                                 params,
                                 paramLengths,
                                 paramFormats,
                                 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        db_log_error(res, "dao_sessions_count_active_by_user failed");
        PQclear(res);
        return -1;
    }

    if (PQntuples(res) != 1) {
        PQclear(res);
        return -1;
    }

    int count = atoi(PQgetvalue(res, 0, 0));
    PQclear(res);

    printf("[DAO] User %lld has %d active session(s)\n", (long long)user_id, count);
    return (count > 0) ? 1 : 0;
}

// [FORBID-LOGIN] Deactivate all active sessions for a user
// Used when user logs in from another device
// Returns: 0 = success, -1 = db error
int dao_sessions_deactivate_all_by_user(int64_t user_id) {
    if (!db_is_ok()) return -1;

    // This should not happen with proper client logic, but added for safety
    // In FORBID-LOGIN, we reject login if user has active session
    // This function is only for emergency cleanup

    const char *sql =
        "UPDATE user_sessions "
        "SET expires_at = NOW() "
        "WHERE user_id = $1 AND expires_at > NOW();";

    char user_id_str[32];
    snprintf(user_id_str, sizeof(user_id_str), "%lld", (long long)user_id);

    const char *params[1] = { user_id_str };
    int paramLengths[1]   = { (int)strlen(user_id_str) };
    int paramFormats[1]   = { 0 };

    PGresult *res = PQexecParams(db_conn,
                                 sql,
                                 1,
                                 NULL,
                                 params,
                                 paramLengths,
                                 paramFormats,
                                 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        db_log_error(res, "dao_sessions_deactivate_all_by_user failed");
        PQclear(res);
        return -1;
    }

    int affected = atoi(PQcmdTuples(res));
    PQclear(res);

    printf("[DAO] Deactivated %d session(s) for user %lld\n", affected, (long long)user_id);
    return 0;
}

// [FORBID-LOGIN] Cleanup all active sessions when server restarts
// This prevents the issue where old sessions block new logins after server restart
// Returns: number of sessions cleaned up, -1 = db error
int dao_sessions_cleanup_all_on_restart() {
    if (!db_is_ok()) return -1;

    const char *sql =
        "UPDATE user_sessions "
        "SET expires_at = NOW() "
        "WHERE expires_at > NOW();";

    PGresult *res = PQexec(db_conn, sql);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        db_log_error(res, "dao_sessions_cleanup_all_on_restart failed");
        PQclear(res);
        return -1;
    }

    int affected = atoi(PQcmdTuples(res));
    PQclear(res);

    printf("[INIT] Cleaned up %d stale session(s) from previous server instance\n", affected);
    return affected;
}

// [HEARTBEAT] Find stale user IDs (last_heartbeat > stale_seconds ago)
int dao_sessions_find_stale_users(int stale_seconds, int64_t **out_user_ids) {
    if (!db_is_ok()) return -1;

    const char *sql =
        "SELECT user_id FROM user_sessions "
        "WHERE expires_at > NOW() "
        "  AND EXTRACT(EPOCH FROM (NOW() - last_heartbeat)) > $1;";

    char stale_str[32];
    snprintf(stale_str, sizeof(stale_str), "%d", stale_seconds);

    const char *params[1] = { stale_str };
    int paramLengths[1]   = { (int)strlen(stale_str) };
    int paramFormats[1]   = { 0 };

    PGresult *res = PQexecParams(db_conn,
                                 sql,
                                 1,
                                 NULL,
                                 params,
                                 paramLengths,
                                 paramFormats,
                                 0);

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        db_log_error(res, "dao_sessions_find_stale_users failed");
        PQclear(res);
        return -1;
    }

    int count = PQntuples(res);
    if (count == 0) {
        PQclear(res);
        *out_user_ids = NULL;
        return 0;
    }

    // Allocate array for user_ids
    int64_t *user_ids = malloc(count * sizeof(int64_t));
    if (!user_ids) {
        PQclear(res);
        return -1;
    }

    for (int i = 0; i < count; i++) {
        user_ids[i] = atoll(PQgetvalue(res, i, 0));
    }

    PQclear(res);
    *out_user_ids = user_ids;
    return count;
}

// [HEARTBEAT] Cleanup stale sessions (last_heartbeat > stale_seconds ago)
int dao_sessions_cleanup_stale(int stale_seconds) {
    if (!db_is_ok()) return -1;

    const char *sql =
        "UPDATE user_sessions "
        "SET expires_at = NOW() "
        "WHERE expires_at > NOW() "
        "  AND EXTRACT(EPOCH FROM (NOW() - last_heartbeat)) > $1;";

    char stale_str[32];
    snprintf(stale_str, sizeof(stale_str), "%d", stale_seconds);

    const char *params[1] = { stale_str };
    int paramLengths[1]   = { (int)strlen(stale_str) };
    int paramFormats[1]   = { 0 };

    PGresult *res = PQexecParams(db_conn,
                                 sql,
                                 1,
                                 NULL,
                                 params,
                                 paramLengths,
                                 paramFormats,
                                 0);

    if (PQresultStatus(res) != PGRES_COMMAND_OK) {
        db_log_error(res, "dao_sessions_cleanup_stale failed");
        PQclear(res);
        return -1;
    }

    int affected = atoi(PQcmdTuples(res));
    PQclear(res);

    if (affected > 0) {
        printf("[HEARTBEAT] Cleaned up %d stale session(s) (no activity for %d seconds)\n", 
               affected, stale_seconds);
    }
    return affected;
}
