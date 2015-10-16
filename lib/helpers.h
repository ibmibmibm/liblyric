#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#ifndef likely
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define likely(x)   __builtin_expect(!!(x), 1)
        #define unlikely(x)   __builtin_expect(!!(x), 0)
    #else
        #define likely(x) (x)
        #define unlikely(x) (x)
    #endif
#endif

#ifndef LIBLYRIC_UNUSED
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define LIBLYRIC_UNUSED __attribute__((unused))
    #else
        #define LIBLYRIC_UNUSED
    #endif
#endif

#ifndef LIBLYRIC_MALLOC
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define LIBLYRIC_MALLOC __attribute__((malloc))
    #else
        #define LIBLYRIC_MALLOC
    #endif
#endif

#ifndef LIBLYRIC_WARN_UNUSED_RESULT
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define LIBLYRIC_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
    #else
        #define LIBLYRIC_WARN_UNUSED_RESULT
    #endif
#endif

#define LIBLYRIC_DEFAULT_BUFFER_SIZE 4096

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
static inline void *lyric_alloc(size_t size) {
    void *const pointer = malloc(size);
    return pointer;
}

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
static inline void *lyric_alloc_init(size_t size) {
    void *const pointer = calloc(1, size);
    return pointer;
}

static inline void lyric_free(void *pointer) {
    free(pointer);
}

LIBLYRIC_WARN_UNUSED_RESULT
static inline bool lyric_resize_array(void **pointer, size_t size, size_t *array_size, size_t new_size) {
    void *new_pointer = realloc(*pointer, size * new_size);
    if (unlikely(!new_pointer)) {
        return false;
    }
    *array_size = new_size;
    *pointer = new_pointer;
    return true;
}

static inline bool lyric_extend_array(void **pointer, size_t size, size_t *array_size) {
    return lyric_resize_array(pointer, size, array_size, *array_size == 0 ? 1 : *array_size * 2);
}

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
static inline char *lyric_strndup(const char *const restrict pointer, const size_t size) {
    char *result = lyric_alloc(size + 1);
    if (unlikely(result == NULL)) {
        return NULL;
    }
    strncpy(result, pointer, size);
    result[size] = '\0';
    return result;
}

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
static inline char *lyric_strdup(const char *const restrict pointer) {
    return lyric_strndup(pointer, strlen(pointer));
}

#ifdef HAVE_STRCASECMP
#include <strings.h>
static inline int lyric_strncasecmp(const char *const restrict a, const char *const restrict b, const size_t n) {
    return strncasecmp(a, b, n);
}
#else
static inline int lyric_strncasecmp(const char *const restrict a, const char *const restrict b, const size_t n) {
    for (size_t i = 0; i < n; ++i) {
        const int ac = tolower(a[i]);
        const int bc = tolower(b[i]);
        if (ac < bc) {
            return -1;
        }
        if (ac > bc) {
            return 1;
        }
    }
    return 0;
}
#endif // HAVE_STRCASECMP

static inline void lyric_strreverse(char *restrict begin, char *restrict end) {
    char aux;
    while (begin < end) {
        aux = *begin;
        *begin++ = *end;
        *end-- = aux;
    }
}

static inline void lyric_ultostr(unsigned long int value, size_t padding, char *restrict string, char **restrict endpos) {
    char *cursor = string;
    char *min_endpos = string + padding;
    do {
        *cursor++ = (char)((value % 10) + '0');
        value /= 10;
    } while (value != 0);
    while (cursor < min_endpos) {
        *cursor++ = '0';
    }
    *cursor = '\0';
    if (endpos != NULL) {
        *endpos = cursor;
    }
    lyric_strreverse(string, cursor - 1);
}

#endif // __HELPERS_H__
// vim:ts=4 sts=4 sw=4 et
