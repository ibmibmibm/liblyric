#include "singer.h"
#include "helpers.h"

bool lyric_singer_create(Singer *const restrict singer) {
    if (unlikely(!lyric_tags_create(&singer->tags))) {
        goto err0;
    }
    singer->line_size = 0;
    singer->_malloc_line_size = 1;
    singer->lines = (Line*)lyric_alloc(sizeof(Line));
    if (unlikely(singer->lines == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_tags_clean(&singer->tags);
err0:
    return false;
}

bool lyric_singer_copy(Singer* const restrict singer, const Singer *const restrict _singer) {
    if (unlikely(_singer == NULL || singer == NULL)) {
        goto err0;
    }
    if (unlikely(singer == _singer)) {
        return true;
    }
    if (unlikely(!lyric_tags_copy(&singer->tags, &_singer->tags))) {
        goto err0;
    }
    const size_t size = _singer->line_size;
    singer->_malloc_line_size = size;
    singer->lines = (Line*)lyric_alloc(sizeof(Line) * size);
    if (unlikely(singer->lines == NULL)) {
        goto err1;
    }
    for (singer->line_size = 0; singer->line_size < size; ++singer->line_size) {
        if (unlikely(!lyric_line_copy(&singer->lines[singer->line_size], &_singer->lines[singer->line_size]))) {
            goto err2;
        }
    }
    return true;
err2:
    for (size_t i = 0; i < singer->line_size; ++i) {
        lyric_line_clean(&singer->lines[i]);
    }
    lyric_free(singer->lines);
err1:
    lyric_tags_clean(&singer->tags);
err0:
    return false;
}

Singer* lyric_singer_new(void) {
    Singer *singer = (Singer*)lyric_alloc(sizeof(Singer));
    if (unlikely(singer == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_singer_create(singer))) {
        goto err1;
    }
    return singer;
err1:
    lyric_free(singer);
err0:
    return NULL;
}

Singer* lyric_singer_new_copy(const Singer *const restrict _singer) {
    Singer *singer = (Singer*)lyric_alloc(sizeof(Singer));
    if (unlikely(singer == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_singer_copy(singer, _singer))) {
        goto err1;
    }
    return singer;
err1:
    lyric_free(singer);
err0:
    return NULL;
}

void lyric_singer_clean(Singer *const restrict singer) {
    lyric_tags_clean(&singer->tags);
    if (likely(singer != NULL)) {
        for (size_t i = 0; i < singer->line_size; ++i) {
            lyric_line_clean(&singer->lines[i]);
        }
        lyric_free(singer->lines);
    }
}

void lyric_singer_delete(Singer *const restrict singer) {
    if (unlikely(singer == NULL))
        return;
    lyric_singer_clean(singer);
    lyric_free(singer);
}

bool lyric_singer_insert(Singer *const restrict singer, const size_t position, const Line *const restrict line) {
    if (unlikely(singer->line_size < position || line == NULL))
        return false;
    if (unlikely(singer->line_size == singer->_malloc_line_size)) {
        void *const array = lyric_extend_array(singer->lines, sizeof(Line), &singer->_malloc_line_size);
        if (unlikely(array == NULL)) {
            return false;
        }
        singer->lines = array;
    }
    memmove(singer->lines + position + 1, singer->lines + position, sizeof(Line) * (singer->line_size - position));
    if (unlikely(!lyric_line_copy(&singer->lines[position], line))) {
        memmove(singer->lines + position, singer->lines + position + 1, sizeof(Line) * (singer->line_size - position));
        return false;
    }
    ++singer->line_size;
    return true;
}

void lyric_singer_remove(Singer *const restrict singer, const size_t position, Line *const restrict line) {
    if (unlikely(singer->line_size <= position))
        return;

    if (unlikely(line != NULL)) {
        memcpy(line, &singer->lines[position], sizeof(Line));
    }
    memmove(singer->lines + position, singer->lines + position + 1, sizeof(Line) * (singer->line_size - position));
    --singer->line_size;
}

bool lyric_singer_push_back(Singer *const restrict singer, const Line *const restrict line) {
    return lyric_singer_insert(singer, singer->line_size, line);
}

void lyric_singer_pop_back(Singer *const restrict singer, Line *const restrict line) {
    lyric_singer_remove(singer, singer->line_size - 1, line);
}
// vim:ts=4 sts=4 sw=4 et
