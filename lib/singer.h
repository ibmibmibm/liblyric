#ifndef __SINGER_H__
#define __SINGER_H__

#include <stdio.h>
#include "property.h"

struct _Property;

struct _Singer {
    struct _Property *property;
};
typedef struct _Singer Singer;

Singer* lyric_singer_new(void);
Singer* lyric_singer_new_copy(Singer *singer);
Singer* lyric_singer_new_from_file(FILE *file);
void lyric_singer_save_to_file(const Singer *singer, FILE *file);
void lyric_singer_delete(Singer *singer);

#endif // __SINGER_H__
