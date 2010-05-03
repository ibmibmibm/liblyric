#ifndef __SINGER_H__
#define __SINGER_H__

#include "tag.h"
#include "line.h"

struct _Singer {
    Tag tag;
    Line *lines;
    size_t line_size;
    size_t _malloc_line_size;
};
typedef struct _Singer Singer;

bool lyric_singer_create(Singer *const restrict singer);
bool lyric_singer_copy(Singer* const restrict singer, const Singer *const restrict _singer);
void lyric_singer_clean(Singer *const restrict singer);

Singer* lyric_singer_new(void);
Singer* lyric_singer_new_copy(const Singer *const restrict _singer);
void lyric_singer_delete(Singer *const restrict singer);

bool lyric_singer_insert(Singer *const restrict singer, const size_t position, const Line *const restrict line);
void lyric_singer_remove(Singer *const restrict singer, const size_t position, Line *const restrict line);
bool lyric_singer_push_back(Singer *const restrict singer, const Line *const restrict line);
void lyric_singer_pop_back(Singer *const restrict singer, Line *const restrict line);

#endif // __SINGER_H__
