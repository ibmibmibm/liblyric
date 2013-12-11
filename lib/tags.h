#ifndef __TAG_H__
#define __TAG_H__

#include "helpers.h"

struct _Tags {
    char **name;
    char **value;
    size_t size;
    size_t _malloc_size;
};
typedef struct _Tags Tags;

bool lyric_tags_create(Tags *const restrict tags);
bool lyric_tags_copy(Tags* const restrict tags, const Tags *const restrict _tags);
void lyric_tags_clean(Tags *const restrict tags);

Tags* lyric_tags_new(void);
Tags* lyric_tags_new_copy(const Tags *const restrict _tags);
void lyric_tags_delete(Tags *const restrict tags);

bool lyric_tags_insert(Tags *const restrict tags, const char *const restrict name, const char *const restrict value);
void lyric_tags_remove(Tags *const restrict tags, char *const restrict name);

#endif // __TAG_H__
