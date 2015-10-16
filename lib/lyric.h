#ifndef __LYRIC_H__
#define __LYRIC_H__

#include "helpers.h"
#include "tags.h"
#include "singer.h"
#include "parser.h"

struct _Lyric {
    struct _Tags tags;
    struct _Singer *singers;
    size_t singer_size;
    size_t _malloc_singer_size;
};
typedef struct _Lyric Lyric;

bool lyric_lyric_create(Lyric *const restrict lyric);
bool lyric_lyric_copy(Lyric *const restrict lyric, const Lyric *const restrict _lyric);
void lyric_lyric_clean(Lyric *const restrict lyric);

Lyric *lyric_lyric_new(void);
Lyric *lyric_lyric_new_copy(const Lyric *const restrict _lyric);
void lyric_lyric_delete(Lyric *const restrict lyric);

bool lyric_lyric_insert(Lyric *const restrict lyric, const size_t position, const Singer *const restrict singer);
void lyric_lyric_remove(Lyric *const restrict lyric, const size_t position, Singer *const restrict singer);
bool lyric_lyric_push_back(Lyric *const restrict lyric, const Singer *const restrict singer);
void lyric_lyric_pop_back(Lyric *const restrict lyric, Singer *const restrict singer);

#endif // __LYRIC_H__
// vim:ts=4 sts=4 sw=4 et
