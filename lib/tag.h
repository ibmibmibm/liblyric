#ifndef __TAG_H__
#define __TAG_H__

#include "helpers.h"

struct _Tag {
    char **name;
    char **value;
    size_t size;
    size_t _malloc_size;
};
typedef struct _Tag Tag;

bool lyric_tag_create(Tag *const restrict tag);
bool lyric_tag_copy(Tag* const restrict tag, const Tag *const restrict _tag);
void lyric_tag_clean(Tag *const restrict tag);

Tag* lyric_tag_new(void);
Tag* lyric_tag_new_copy(const Tag *const restrict _tag);
void lyric_tag_delete(Tag *const restrict tag);

bool lyric_tag_insert(Tag *const restrict tag, const char *const restrict name, const char *const restrict value);
void lyric_tag_remove(Tag *const restrict tag, char *const restrict name);

#endif // __TAG_H__
