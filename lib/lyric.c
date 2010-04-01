#include "lyric.h"
#include "helpers.h"

bool lyric_lyric_create(Lyric *const restrict lyric) {
    if (unlikely(!lyric_tag_create(&lyric->tag))) {
        goto err0;
    }
    lyric->singer_size = 0;
    lyric->_malloc_singer_size = 1;
    lyric->singers = (Singer*)lyric_alloc(sizeof(Singer));
    if (unlikely(lyric->singers == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_tag_clean(&lyric->tag);
err0:
    return false;
}

bool lyric_lyric_copy(Lyric* const restrict lyric, const Lyric *const restrict _lyric) {
    if (unlikely(_lyric == NULL || lyric == NULL)) {
        goto err0;
    }
    if (unlikely(lyric == _lyric)) {
        return true;
    }
    if (unlikely(!lyric_tag_copy(&lyric->tag, &_lyric->tag))) {
        goto err0;
    }
    const size_t size = _lyric->singer_size;
    lyric->_malloc_singer_size = size;
    lyric->singers = (Singer*)lyric_alloc(sizeof(Singer) * size);
    if (unlikely(lyric->singers == NULL)) {
        goto err1;
    }
    for (lyric->singer_size = 0; lyric->singer_size < lyric->singer_size; ++lyric->singer_size) {
        if (unlikely(!lyric_singer_copy(&lyric->singers[lyric->singer_size], &_lyric->singers[lyric->singer_size]))) {
            goto err2;
        }
    }
    return true;
err2:
    for (size_t i = 0; i < lyric->singer_size; ++i) {
        lyric_singer_clean(&lyric->singers[i]);
    }
    lyric_free(lyric->singers);
err1:
    lyric_tag_clean(&lyric->tag);
err0:
    return false;
}

Lyric* lyric_lyric_new(void) {
    Lyric *lyric = (Lyric*)lyric_alloc(sizeof(Lyric));
    if (unlikely(lyric == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_lyric_create(lyric))) {
        goto err1;
    }
    return lyric;
err1:
    lyric_free(lyric);
err0:
    return NULL;
}

Lyric* lyric_lyric_new_copy(const Lyric *const restrict _lyric) {
    Lyric *lyric = (Lyric*)lyric_alloc(sizeof(Lyric));
    if (unlikely(lyric == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_lyric_copy(lyric, _lyric))) {
        goto err1;
    }
    return lyric;
err1:
    lyric_free(lyric);
err0:
    return NULL;
}

void lyric_lyric_clean(Lyric *const restrict lyric) {
    lyric_tag_clean(&lyric->tag);
    if (likely(lyric != NULL)) {
        for (size_t i = 0; i < lyric->singer_size; ++i) {
            lyric_singer_clean(&lyric->singers[i]);
        }
        lyric_free(lyric->singers);
    }
}

void lyric_lyric_delete(Lyric *const restrict lyric) {
    if (unlikely(lyric == NULL))
        return;
    lyric_lyric_clean(lyric);
    lyric_free(lyric);
}

bool lyric_lyric_insert(Lyric *const restrict lyric, const size_t position, const Singer *const restrict singer) {
    if (unlikely(lyric->singer_size < position || singer == NULL))
        return false;
    if (unlikely(lyric->singer_size == lyric->_malloc_singer_size)) {
        void *const array = lyric_extend_array(lyric->singers, sizeof(Singer), &lyric->_malloc_singer_size);
        if (unlikely(array == NULL)) {
            return false;
        }
        lyric->singers = array;
    }
    memmove(lyric->singers + position + 1, lyric->singers + position, sizeof(Singer) * (lyric->singer_size - position));
    if (unlikely(!lyric_singer_copy(&lyric->singers[position], singer))) {
        memmove(lyric->singers + position, lyric->singers + position + 1, sizeof(Singer) * (lyric->singer_size - position));
        return false;
    }
    ++lyric->singer_size;
    return true;
}

void lyric_lyric_remove(Lyric *const restrict lyric, const size_t position, Singer *const restrict singer) {
    if (unlikely(lyric->singer_size <= position))
        return;

    if (unlikely(singer != NULL)) {
        memcpy(singer, &lyric->singers[position], sizeof(Singer));
    }
    memmove(lyric->singers + position, lyric->singers + position + 1, sizeof(Singer) * (lyric->singer_size - position));
    --lyric->singer_size;
}

bool lyric_lyric_push_back(Lyric *const restrict lyric, const Singer *const restrict singer) {
    return lyric_lyric_insert(lyric, lyric->singer_size, singer);
}

void lyric_lyric_pop_back(Lyric *const restrict lyric, Singer *const restrict singer) {
    return lyric_lyric_remove(lyric, lyric->singer_size - 1, singer);
}
