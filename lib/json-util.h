#ifndef __JSON_UTIL_H__
#define __JSON_UTIL_H__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define json_max(a,b) ((a) > (b) ? (a) : (b))
#define json_min(a,b) ((a) < (b) ? (a) : (b))

static int strncasecmp(const char *const restrict a, const char *const restrict b, const size_t n) {
    for (size_t i = 0; i < n; ++i) {
        const int ac = tolower(a[i]);
        const int bc = tolower(b[i]);
        if (ac < bc)
            return -1;
        if (ac > bc)
            return 1;
    }
    return 0;
}

static char* strdup(const char* str) {
    if (str) {
        size_t len = strlen(str);
        char *s = (char*)malloc(sizeof(char) * (len + 1));
        if (s) {
            memcpy(s, str, len);
            s[len] = '\0';
        }
        return s;
    }
    return NULL;
}

static char* strndup(const char* str, size_t n) {
    if (str) {
        size_t len = strlen(str);
        size_t nn = json_min(len, n);
        char *s = (char*)malloc(sizeof(char) * (nn + 1));
        if (s) {
            memcpy(s, str, nn);
            s[nn] = '\0';
        }
        return s;
    }
    return NULL;
}

extern struct json_object* json_object_from_file(FILE *file);

#endif // __JSON_UTIL_H__
