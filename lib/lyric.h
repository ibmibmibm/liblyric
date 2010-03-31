#ifndef __LYRIC_H__
#define __LYRIC_H__

#include <stdio.h>
#include <stdbool.h>
#include "property.h"
#include "singer.h"

struct _Property;
struct _Singer;
struct _Lyric {
    struct _Property *property;
    struct _Singer **singers;
    size_t singer_size;
    size_t _malloc_singer_size;
};
typedef struct _Lyric Lyric;

Lyric* lyric_lyric_new(void);
Lyric* lyric_lyric_new_copy(const Lyric *_lyric);
Lyric* lyric_lyric_new_from_file(FILE *file);
void lyric_lyric_save_to_file(const Lyric *lyric, FILE *file);
void lyric_lyric_delete(Lyric *lyric);
bool lyric_lyric_insert(Lyric *lyric, const size_t position, struct _Singer *singer);
void lyric_lyric_remove(Lyric *lyric, const size_t position, struct _Singer **singer);
bool lyric_lyric_push_back(Lyric *lyric, struct _Singer *singer);
void lyric_lyric_pop_back(Lyric *lyric, struct _Singer **singer);

#endif // __LYRIC_H__
