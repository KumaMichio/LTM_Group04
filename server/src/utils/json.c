#include "utils/json.h"
#include <stdlib.h>
#include <string.h>


char *util_json_escape(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s);
    size_t out_cap = len * 2 + 1;
    char *out = malloc(out_cap);
    if (!out) return NULL;
    size_t o = 0;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
            case '"': case '\\': out[o++] = '\\'; out[o++] = c; break;
            case '\n': out[o++] = '\\'; out[o++] = 'n'; break;
            case '\r': out[o++] = '\\'; out[o++] = 'r'; break;
            case '\t': out[o++] = '\\'; out[o++] = 't'; break;
            default:
                if (c < 0x20) {
                    const char *hex = "0123456789abcdef";
                    if (o + 6 >= out_cap) {
                        // expand
                        out_cap *= 2;
                        char *tmp = realloc(out, out_cap);
                        if (!tmp) { free(out); return NULL; }
                        out = tmp;
                    }
                    out[o++] = '\\'; out[o++] = 'u'; out[o++] = '0'; out[o++] = '0';
                    out[o++] = hex[(c >> 4) & 0xF]; out[o++] = hex[c & 0xF];
                } else {
                    out[o++] = c;
                }
        }
        if (o + 1 >= out_cap) {
            out_cap *= 2;
            char *tmp = realloc(out, out_cap);
            if (!tmp) { free(out); return NULL; }
            out = tmp;
        }
    }
    out[o] = '\0';
    return out;
}

char *util_json_get_string(const char *json, const char *key) {
    if (!json || !key) return NULL;
    // find "key"
    const char *p = strstr(json, key);
    if (!p) return NULL;
    // find colon after key
    p = strchr(p, ':');
    if (!p) return NULL;
    // find starting quote
    p = strchr(p, '"');
    if (!p) return NULL;
    ++p;
    const char *q = p;
    while (*q && *q != '"') {
        if (*q == '\\' && *(q+1)) q += 2; else q++;
    }
    if (!*q) return NULL;
    size_t len = q - p;
    char *out = malloc(len + 1);
    if (!out) return NULL;
    strncpy(out, p, len);
    out[len] = '\0';
    return out;
}

int util_json_get_int64(const char *json, const char *key, long long *out) {
    if (!json || !key || !out) return 0;
    const char *p = strstr(json, key);
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    ++p;
    // skip spaces
    while (*p == ' ' || *p == '\t') ++p;
    char *end;
    long long v = strtoll(p, &end, 10);
    if (p == end) return 0;
    *out = v;
    return 1;
}

int util_json_get_double(const char *json, const char *key, double *out) {
    if (!json || !key || !out) return 0;
    const char *p = strstr(json, key);
    if (!p) return 0;
    p = strchr(p, ':');
    if (!p) return 0;
    ++p;
    // skip spaces
    while (*p == ' ' || *p == '\t') ++p;
    char *end;
    double v = strtod(p, &end);
    if (p == end) return 0;
    *out = v;
    return 1;
}

int util_json_parse_user_id_array(const char *json, int64_t *out_array, int max_count) {
    return util_json_parse_int64_array(json, "user_id", out_array, max_count);
}

int util_json_parse_int64_array(const char *json, const char *field_name, int64_t *out_array, int max_count) {
    if (!json || !field_name || !out_array || max_count <= 0) return 0;
    
    int count = 0;
    const char *p = json;
    
    // Find opening bracket
    p = strchr(p, '[');
    if (!p) return 0;
    p++;
    
    while (count < max_count && *p) {
        // Skip whitespace
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') p++;
        if (*p == ']') break;
        if (*p == ',') p++;
        
        // Find field_name
        const char *field_pos = strstr(p, field_name);
        if (!field_pos) break;
        
        // Find colon after field
        field_pos = strchr(field_pos, ':');
        if (!field_pos) break;
        field_pos++;
        
        // Skip whitespace
        while (*field_pos == ' ' || *field_pos == '\t') field_pos++;
        
        // Parse number
        char *end;
        long long value = strtoll(field_pos, &end, 10);
        if (field_pos != end) {
            out_array[count++] = value;
        }
        
        // Move to next object (find closing brace)
        p = strchr(field_pos, '}');
        if (!p) break;
        p++;
    }
    
    return count;
}