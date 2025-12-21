#ifndef UTIL_JSON_H
#define UTIL_JSON_H

#include <stdint.h>

// Escape a C string for JSON and return a newly allocated string.
// Caller must free the returned pointer.
char *util_json_escape(const char *s);

// Very small helper: extract a JSON string field value by key ("key": "value")
// Returns newly allocated string with the value or NULL if not found.
char *util_json_get_string(const char *json, const char *key);

// Get integer value for key (e.g. "friend_id": 123). Returns 1 on success and stores value in out, 0 otherwise.
int util_json_get_int64(const char *json, const char *key, long long *out);

// Get double value for key (e.g. "time_left": 12.5). Returns 1 on success and stores value in out, 0 otherwise.
int util_json_get_double(const char *json, const char *key, double *out);

// Parse JSON array of objects with user_id field
// Returns number of user_ids found, stores in out_array (max max_count)
// Example: [{"user_id": 1}, {"user_id": 2}] -> returns 2, out_array = [1, 2]
int util_json_parse_user_id_array(const char *json, int64_t *out_array, int max_count);

// Parse JSON array of objects with any int64 field
// Returns number of values found
int util_json_parse_int64_array(const char *json, const char *field_name, int64_t *out_array, int max_count);

#endif
