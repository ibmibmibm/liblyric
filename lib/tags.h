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

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_tags_create(Tags *const restrict tags);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_tags_copy(Tags *const restrict tags, const Tags *const restrict _tags);

void lyric_tags_clean(Tags *const restrict tags);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
Tags *lyric_tags_new(void);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
Tags *lyric_tags_new_copy(const Tags *const restrict _tags);

void lyric_tags_delete(Tags *const restrict tags);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_tags_insert(Tags *const restrict tags, const char *const restrict name, const char *const restrict value);

void lyric_tags_remove(Tags *const restrict tags, char *const restrict name);

#endif // __TAG_H__
// vim:ts=4 sts=4 sw=4 et
