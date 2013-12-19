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

#define lyric_min(a, b) ((a) < (b) ? (a) : (b))

#ifdef LEAK_LOG
static inline void lyric_trace(void) {
    void *array[10];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace(array, 10);
    strings = backtrace_symbols(array, size);

    fprintf(stderr, "Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++)
        fprintf(stderr, "%s\n", strings[i]);

    free(strings);
}
#endif // LEAK_LOG

static inline void *lyric_alloc(size_t size) {
    void *const pointer = malloc(size);
#ifdef LEAK_LOG
    fprintf(stderr, "+%p %zd\n", pointer, size);
#ifdef BACKTRACE
    lyric_trace();
#endif // BACKTRACE
#endif // LEAK_LOG
    return pointer;
}

static inline void *lyric_alloc_init(size_t size) {
    void *const pointer = calloc(1, size);
#ifdef LEAK_LOG
    fprintf(stderr, "+%p %zd\n", pointer, size);
#endif // LEAK_LOG
    return pointer;
}

static inline void lyric_free(void *pointer) {
#ifdef LEAK_LOG
    fprintf(stderr, "-%p\n", pointer);
#endif // LEAK_LOG
    free(pointer);
}

static inline void *lyric_resize_array(void *pointer, size_t size, size_t *array_size, size_t new_size) {
    void *new_pointer = realloc(pointer, size * new_size);
    if (new_pointer) {
#ifdef LEAK_LOG
        if (pointer != new_pointer) {
            fprintf(stderr, "-%p\n", pointer);
            fprintf(stderr, "+%p %zd\n", new_pointer, size);
        } else {
            fprintf(stderr, "=%p %zd\n", new_pointer, size);
        }
#endif // LEAK_LOG
        *array_size = new_size;
    }
    return new_pointer;
}

static inline void *lyric_extend_array(void *pointer, size_t size, size_t *array_size) {
    return lyric_resize_array(pointer, size, array_size, *array_size == 0 ? 1 : *array_size * 2);
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

static inline void lyric_strreverse(char*restrict begin, char*restrict end) {
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
        *cursor++ = (char) ((value % 10) + '0');
        value /= 10;
    } while (value != 0);
    while (cursor < min_endpos) {
        *cursor++ = '0';
    }
    *cursor = '\0';
    if (endpos != NULL)
        *endpos = cursor;
    lyric_strreverse(string, cursor - 1);
}

#endif // __HELPERS_H__
// vim:ts=4 sts=4 sw=4 et
