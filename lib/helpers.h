#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#ifndef likely
    #ifdef __GNUC__
        #if __GNUC__ < 3
            #define __builtin_expect(x, n) (x)
        #endif

        #define likely(x)   __builtin_expect(!!(x), 1)
        #define unlikely(x)   __builtin_expect(!!(x), 0)
    #else
        #define likely(x) x
        #define unlikely(x) x
    #endif
#endif

#define lyric_min(a, b) ((a) < (b) ? (a) : (b))

static inline void *lyric_alloc(size_t size) {
    return malloc(size);
}

static inline void *lyric_alloc_init(size_t size) {
    return calloc(size, 1);
}

static inline void lyric_free(void *pointer) {
    free(pointer);
}

static inline void *lyric_resize_array(void *pointer, size_t size, size_t *array_size, size_t new_size) {
    void *new_pointer = realloc(pointer, size * new_size);
    if (new_pointer)
        *array_size = new_size;
    return new_pointer;
}

static inline void *lyric_extend_array(void *pointer, size_t size, size_t *array_size) {
    return lyric_resize_array(pointer, size, array_size, *array_size * 2);
}

static inline char *lyric_strdup(const char *const restrict pointer) {
    char *result = lyric_alloc(strlen(pointer) + 1);
    if (unlikely(result == NULL))
        return NULL;
    strcpy(result, pointer);
    return result;
}

static inline char *lyric_strndup(const char *const restrict pointer, const size_t size) {
    char *result = lyric_alloc(size + 1);
    if (unlikely(result == NULL))
        return NULL;
    strncpy(result, pointer, size);
    result[size] = '\0';
    return result;
}

static inline int lyric_strncasecmp(const char *const restrict a, const char *const restrict b, const size_t n) {
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

#endif // __HELPERS_H__
