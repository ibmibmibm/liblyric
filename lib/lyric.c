#include <stdlib.h>
#include <string.h>
#include "lyric.h"
#include "helpers.h"
#include "property.h"

static bool _extend_array(Lyric *lyric);

Lyric* lyric_lyric_new(void) {
    Lyric *lyric = (Lyric*)calloc(sizeof(Lyric), 1);
    if (lyric == NULL) {
        return NULL;
    }
    lyric->property = lyric_property_new();
    if (lyric->property == NULL) {
        lyric_lyric_delete(lyric);
        return NULL;
    }
    lyric->singer_size = 0;
    lyric->_malloc_singer_size = 1;
    lyric->singers = (Singer**)malloc(sizeof(Singer*));
    if (lyric->singers == NULL) {
        lyric_lyric_delete(lyric);
        return NULL;
    }
    return lyric;
}

Lyric* lyric_lyric_new_copy(const Lyric *_lyric) {
    size_t i;
    Lyric *lyric = (Lyric*)calloc(sizeof(Lyric), 1);
    if (lyric == NULL) {
        return NULL;
    }
    lyric->property = lyric_property_new_copy(_lyric->property);
    if (lyric->property == NULL) {
        lyric_lyric_delete(lyric);
        return NULL;
    }
    lyric->singer_size = _lyric->singer_size;
    lyric->_malloc_singer_size = _lyric->_malloc_singer_size;
    lyric->singers = (Singer**)calloc(sizeof(Singer*) * lyric->_malloc_singer_size, 1);
    if (lyric->singers == NULL) {
        lyric_lyric_delete(lyric);
        return NULL;
    }
    for (i = 0; i < lyric->singer_size; ++i) {
        lyric->singers[i] = lyric_singer_new_copy(_lyric->singers[i]);
        if (lyric->singers[i] == NULL) {
            lyric_lyric_delete(lyric);
            return NULL;
        }
    }
    return lyric;
}

Lyric* lyric_lyric_new_from_file(FILE *file) {
    Lyric *lyric = (Lyric*)malloc(sizeof(Lyric));
    if (lyric == NULL) {
        return NULL;
    }
    if (!_match_token(file, "lyric"))
        return NULL;
    if (!_match_token(file, "{"))
        return NULL;

    lyric->property = lyric_property_new_from_file(file);
    if (lyric->property == NULL) {
        lyric_lyric_delete(lyric);
        return NULL;
    }

    lyric->singer_size = 0;
    lyric->_malloc_singer_size = 1;
    lyric->singers = (Singer**)malloc(sizeof(Singer*));
    if (lyric->singers == NULL) {
        lyric_lyric_delete(lyric);
        return NULL;
    }
    if (!_match_token(file, "singerlist"))
        return NULL;
    if (!_match_token(file, "{"))
        return NULL;
    while (_match_token(file, "singer")) {
    }
    /* XXX: hacked */
    /*
    if (!_match_token(file, "}")) {
        lyric_lyric_delete(lyric);
        return NULL;
    }
    */
    return lyric;
}

void lyric_lyric_save_to_file(const Lyric *lyric, FILE *file) {
    size_t i;
    fputs("lyric {\n", file);
    lyric_property_save_to_file(lyric->property, file);
    for (i = 0; i < lyric->singer_size; ++i)
        lyric_singer_save_to_file(lyric->singers[i], file);
    fputs("}\n", file);
}

void lyric_lyric_delete(Lyric *lyric) {
    size_t i;
    if (lyric == NULL)
        return;
    lyric_property_delete(lyric->property);
    if (lyric->singers != NULL) {
        for (i = 0; i < lyric->singer_size; ++i)
            lyric_singer_delete(lyric->singers[i]);
        free(lyric->singers);
    }
    free(lyric);
}

bool lyric_lyric_insert(Lyric *lyric, const size_t position, Singer *singer) {
    size_t i;
    Singer *new_singer;
    if (lyric->singer_size < position)
        return false;
    if (lyric->singer_size > lyric->_malloc_singer_size)
        if (!_extend_array(lyric))
            return false;
    new_singer = lyric_singer_new_copy(singer);
    if (new_singer == NULL)
        return false;
    for (i = lyric->singer_size; i > position; --i) {
        Singer *temp;
        temp = lyric->singers[i];
        lyric->singers[i] = lyric->singers[i - 1];
        lyric->singers[i - 1] = temp;
    }
    lyric->singers[i] = new_singer;
    ++lyric->singer_size;
    return true;
}

void lyric_lyric_remove(Lyric *lyric, const size_t position, Singer **singer) {
    size_t i;

    if (lyric->singer_size <= position)
        return;

    if (singer == NULL)
        free(lyric->singers[position]);
    else
        *singer = lyric->singers[position];

    for (i = position + 1; i < lyric->singer_size; ++i) {
        lyric->singers[i - 1] = lyric->singers[i];
    }
    --lyric->singer_size;
}

bool lyric_lyric_push_back(Lyric *lyric, Singer *singer) {
    return lyric_lyric_insert(lyric, lyric->singer_size, singer);
}

void lyric_lyric_pop_back(Lyric *lyric, Singer **singer) {
    return lyric_lyric_remove(lyric, lyric->singer_size - 1, singer);
}

static bool _extend_array(Lyric *lyric) {
    size_t new_malloc_size = lyric->_malloc_singer_size * 2;
    Singer **new_singer_array;
    new_singer_array = (Singer**) realloc(lyric->singers, sizeof(Singer*) * new_malloc_size);
    if (new_singer_array == NULL)
        return false;
    lyric->singers = new_singer_array;
    lyric->_malloc_singer_size = new_malloc_size;
    return true;
}
