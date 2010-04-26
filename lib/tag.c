#include "tag.h"

bool lyric_tag_create(Tag *const restrict tag) {
    tag->size = 0;
    tag->_malloc_size = 1;
    tag->name = (char**)lyric_alloc(sizeof(char*));
    if (unlikely(tag->name == NULL)) {
        goto err0;
    }
    tag->value = (char**)lyric_alloc(sizeof(char*));
    if (unlikely(tag->value == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_free(tag->name);
err0:
    return false;
}

bool lyric_tag_copy(Tag* const restrict tag, const Tag *const restrict _tag) {
    if (unlikely(_tag == NULL || tag == NULL)) {
        goto err0;
    }
    if (unlikely(tag == _tag)) {
        return true;
    }
    const size_t size = _tag->size;
    tag->_malloc_size = size;
    tag->name = (char**)lyric_alloc(sizeof(char*) * size);
    if (unlikely(tag->name == NULL)) {
        goto err0;
    }
    tag->value = (char**)lyric_alloc(sizeof(char*) * size);
    if (unlikely(tag->value == NULL)) {
        goto err1;
    }
    for (tag->size = 0; tag->size < size; ++tag->size) {
        tag->name[tag->size] = lyric_strdup(_tag->name[tag->size]);
        if (unlikely(tag->name[tag->size] == NULL)) {
            goto err2;
        }
        tag->value[tag->size] = lyric_strdup(_tag->value[tag->size]);
        if (unlikely(tag->value[tag->size] == NULL)) {
            lyric_free(tag->name[tag->size]);
            goto err2;
        }
    }
    return true;
err2:
    for (size_t i = 0; i < tag->size; ++i) {
        lyric_free(&tag->name[i]);
        lyric_free(&tag->value[i]);
    }
    lyric_free(tag->value);
err1:
    lyric_free(tag->name);
err0:
    return false;
}

Tag* lyric_tag_new(void) {
    Tag *tag = (Tag*)lyric_alloc(sizeof(Tag));
    if (unlikely(tag == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_tag_create(tag))) {
        goto err1;
    }
    return tag;
err1:
    lyric_free(tag);
err0:
    return NULL;
}

Tag* lyric_tag_new_copy(const Tag *const restrict _tag) {
    Tag *tag = (Tag*)lyric_alloc(sizeof(Tag));
    if (unlikely(tag == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_tag_copy(tag, _tag))) {
        goto err1;
    }
    return tag;
err1:
    lyric_free(tag);
err0:
    return NULL;
}

void lyric_tag_clean(Tag *const restrict tag) {
    if (likely(tag != NULL)) {
        for (size_t i = 0; i < tag->size; ++i) {
            lyric_free(tag->name[i]);
            lyric_free(tag->value[i]);
        }
        lyric_free(tag->name);
        lyric_free(tag->value);
    }
}

void lyric_tag_delete(Tag *const restrict tag) {
    if (unlikely(tag == NULL))
        return;
    lyric_tag_clean(tag);
    lyric_free(tag);
}

static size_t _find_name(Tag *const restrict tag, const char *const restrict name, bool *const restrict exact) {
    *exact = false;
    if (tag->size == 0)
        return 0;
    else {
        size_t begin = 0, end = tag->size - 1;
        while (begin < end) {
            size_t mid = (end - begin) / 2 + begin;
            int cmp = strcmp(tag->name[mid], name);
            if (cmp > 0) {
                begin = mid;
            } else if (cmp < 0) {
                end = mid;
            } else {
                *exact = true;
                return mid;
            }
        }
        return begin;
    }
}

bool lyric_tag_insert(Tag *const restrict tag, const char *const restrict name, const char *const restrict value) {
    if (unlikely(tag == NULL || name == NULL || value == NULL))
        return false;
    bool exact;
    const size_t position = _find_name(tag, name, &exact);
    if (exact) {
        char *new_name = lyric_strdup(name);
        if (unlikely(new_name == NULL)) {
            return false;
        }
        char *new_value = lyric_strdup(value);
        if (unlikely(new_value == NULL)) {
            lyric_free(new_value);
            return false;
        }
        lyric_free(tag->name[position]);
        lyric_free(tag->value[position]);
        tag->name[position] = new_name;
        tag->value[position] = new_value;
        return true;
    }
    if (unlikely(tag->size == tag->_malloc_size)) {
        size_t size = tag->_malloc_size;
        void *const name_array = lyric_extend_array(tag->name, sizeof(char*), &size);
        if (unlikely(name_array == NULL)) {
            return false;
        }
        tag->name = name_array;
        size = tag->_malloc_size;
        void *const value_array = lyric_extend_array(tag->value, sizeof(char*), &size);
        if (unlikely(value_array == NULL)) {
            return false;
        }
        tag->value = value_array;
        tag->_malloc_size = size;
    }
    memmove(tag->name + position + 1, tag->name + position, sizeof(char*) * (tag->size - position));
    tag->name[position] = lyric_strdup(name);
    if (unlikely(tag->name[position] == NULL)) {
        goto err1;
    }
    memmove(tag->value + position + 1, tag->value + position, sizeof(char*) * (tag->size - position));
    tag->value[position] = lyric_strdup(value);
    if (unlikely(tag->value[position] == NULL)) {
        goto err0;
    }
    ++tag->size;
    return true;
err1:
    memmove(tag->value + position, tag->value + position + 1, sizeof(char*) * (tag->size - position));
    lyric_free(tag->name[position]);
err0:
    memmove(tag->name + position, tag->name + position + 1, sizeof(char*) * (tag->size - position));
    return false;
}

void lyric_tag_remove(Tag *const restrict tag, char *const restrict name) {
    if (unlikely(name == NULL))
        return;
    size_t begin = 0, end = tag->size - 1;
    while (begin < end) {
        size_t mid = (end - begin) / 2 + begin;
        int cmp = strcmp(tag->name[mid], name);
        if (cmp > 0) {
            begin = mid;
        } else if (cmp < 0) {
            end = mid;
        } else {
            lyric_free(tag->name[mid]);
            lyric_free(tag->value[mid]);
            memmove(tag->name + mid, tag->name + mid + 1, sizeof(char*) * (tag->size - mid));
            memmove(tag->value + mid, tag->value + mid + 1, sizeof(char*) * (tag->size - mid));
            --tag->size;
            return;
        }
    }
}

