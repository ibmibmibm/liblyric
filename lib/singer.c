#include <stdlib.h>
#include "singer.h"

Singer* lyric_singer_new(void) {
    Singer *singer = (Singer*)calloc(sizeof(Singer), 1);
    if (singer == NULL)
        return NULL;
    singer->property = lyric_property_new();
    if (singer->property == NULL) {
        lyric_singer_delete(singer);
        return NULL;
    }
    return singer;
}

Singer* lyric_singer_new_copy(Singer *_singer) {
    Singer *singer = (Singer*)calloc(sizeof(Singer), 1);
    if (singer == NULL)
        return NULL;
    singer->property = lyric_property_new_copy(_singer->property);
    if (singer->property == NULL) {
        lyric_singer_delete(singer);
        return NULL;
    }
    return singer;
}

Singer* lyric_singer_new_from_file(FILE *file) {
    Singer *singer = (Singer*)calloc(sizeof(Singer), 1);
    if (singer == NULL)
        return NULL;
    singer->property = lyric_property_new_from_file(file);
    if (singer->property == NULL) {
        lyric_singer_delete(singer);
        return NULL;
    }
    return singer;
}

void lyric_singer_save_to_file(const Singer *singer, FILE *file) {
    if (singer == NULL)
        return;
    fputs("singer {\n", file);
    lyric_property_save_to_file(singer->property, file);
    fputs("}\n", file);
}

void lyric_singer_delete(Singer *singer) {
    if (singer == NULL)
        return;
    lyric_property_delete(singer->property);
    free(singer);
}
