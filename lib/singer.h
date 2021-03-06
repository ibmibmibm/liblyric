#ifndef __SINGER_H__
#define __SINGER_H__

#include "tags.h"
#include "line.h"

struct _Singer {
    Tags tags;
    Line *lines;
    size_t line_size;
    size_t _malloc_line_size;
};
typedef struct _Singer Singer;

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_singer_create(Singer *const restrict singer);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_singer_copy(Singer *const restrict singer, const Singer *const restrict _singer);

void lyric_singer_clean(Singer *const restrict singer);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
Singer *lyric_singer_new(void);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
Singer *lyric_singer_new_copy(const Singer *const restrict _singer);

void lyric_singer_delete(Singer *const restrict singer);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_singer_insert(Singer *const restrict singer, const size_t position, const Line *const restrict line);

void lyric_singer_remove(Singer *const restrict singer, const size_t position, Line *const restrict line);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_singer_push_back(Singer *const restrict singer, const Line *const restrict line);

void lyric_singer_pop_back(Singer *const restrict singer, Line *const restrict line);

#endif // __SINGER_H__
// vim:ts=4 sts=4 sw=4 et
