#ifndef __HELPERS_H__
#define __HELPERS_H__

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#ifdef BACKTRACE
#include <execinfo.h>
#endif // BACKTRACE

#ifndef likely
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define likely(x)   __builtin_expect(!!(x), 1)
        #define unlikely(x)   __builtin_expect(!!(x), 0)
    #else
        #define likely(x) (x)
        #define unlikely(x) (x)
    #endif
#endif

#ifndef __unused
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define __unused __attribute__((unused))
    #else
        #define __unused
    #endif
#endif

#ifndef __malloc
    #if defined(__GNUC__) && __GNUC__ >= 3
        #define __malloc __attribute__((malloc))
    #else
        #define __malloc
    #endif
#endif

#define lyric_min(a, b) ((a) < (b) ? (a) : (b))

static inline void *lyric_alloc(size_t size) __malloc {
    void *const pointer = malloc(size);
    return pointer;
}

static inline void *lyric_alloc_init(size_t size) __malloc {
    void *const pointer = calloc(1, size);
    return pointer;
}

static inline void lyric_free(void *pointer) {
    free(pointer);
}

static inline void *lyric_resize_array(void *pointer, size_t size, size_t *array_size, size_t new_size) {
    void *new_pointer = realloc(pointer, size * new_size);
    if (new_pointer) {
        *array_size = new_size;
    }
    return new_pointer;
}

static inline void *lyric_extend_array(void *pointer, size_t size, size_t *array_size) {
    return lyric_resize_array(pointer, size, array_size, *array_size == 0 ? 1 : *array_size * 2);
}

static inline char *lyric_strndup(const char *const restrict pointer, const size_t size) {
    char *result = lyric_alloc(size + 1);
    if (unlikely(result == NULL)) {
        return NULL;
    }
    strncpy(result, pointer, size);
    result[size] = '\0';
    return result;
}

static inline char *lyric_strdup(const char *const restrict pointer) {
    return lyric_strndup(pointer, strlen(pointer))
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
