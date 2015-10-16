#include "tags.h"

bool lyric_tags_create(Tags *const restrict tags) {
    tags->size = 0;
    tags->_malloc_size = 1;
    tags->name = (char **)lyric_alloc(sizeof(char *));
    if (unlikely(tags->name == NULL)) {
        goto err0;
    }
    tags->value = (char **)lyric_alloc(sizeof(char *));
    if (unlikely(tags->value == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_free(tags->name);
err0:
    return false;
}

bool lyric_tags_copy(Tags *const restrict tags, const Tags *const restrict _tags) {
    if (unlikely(_tags == NULL || tags == NULL)) {
        goto err0;
    }
    if (unlikely(tags == _tags)) {
        return true;
    }
    const size_t size = _tags->size;
    tags->_malloc_size = size;
    tags->name = (char **)lyric_alloc(sizeof(char *) * size);
    if (unlikely(tags->name == NULL)) {
        goto err0;
    }
    tags->value = (char **)lyric_alloc(sizeof(char *) * size);
    if (unlikely(tags->value == NULL)) {
        goto err1;
    }
    for (tags->size = 0; tags->size < size; ++tags->size) {
        tags->name[tags->size] = lyric_strdup(_tags->name[tags->size]);
        if (unlikely(tags->name[tags->size] == NULL)) {
            goto err2;
        }
        tags->value[tags->size] = lyric_strdup(_tags->value[tags->size]);
        if (unlikely(tags->value[tags->size] == NULL)) {
            lyric_free(tags->name[tags->size]);
            goto err2;
        }
    }
    return true;
err2:
    for (size_t i = 0; i < tags->size; ++i) {
        lyric_free(&tags->name[i]);
        lyric_free(&tags->value[i]);
    }
    lyric_free(tags->value);
err1:
    lyric_free(tags->name);
err0:
    return false;
}

Tags *lyric_tags_new(void) {
    Tags *tags = (Tags *)lyric_alloc(sizeof(Tags));
    if (unlikely(tags == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_tags_create(tags))) {
        goto err1;
    }
    return tags;
err1:
    lyric_free(tags);
err0:
    return NULL;
}

Tags *lyric_tags_new_copy(const Tags *const restrict _tags) {
    Tags *tags = (Tags *)lyric_alloc(sizeof(Tags));
    if (unlikely(tags == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_tags_copy(tags, _tags))) {
        goto err1;
    }
    return tags;
err1:
    lyric_free(tags);
err0:
    return NULL;
}

void lyric_tags_clean(Tags *const restrict tags) {
    if (likely(tags != NULL)) {
        for (size_t i = 0; i < tags->size; ++i) {
            lyric_free(tags->name[i]);
            lyric_free(tags->value[i]);
        }
        lyric_free(tags->name);
        lyric_free(tags->value);
    }
}

void lyric_tags_delete(Tags *const restrict tags) {
    if (unlikely(tags == NULL)) {
        return;
    }
    lyric_tags_clean(tags);
    lyric_free(tags);
}

static size_t _find_name(Tags *const restrict tags, const char *const restrict name, bool *const restrict exact) {
    *exact = false;
    if (tags->size == 0) {
        return 0;
    } else {
        size_t begin = 0, end = tags->size - 1;
        while (begin < end) {
            size_t mid = (end - begin) / 2 + begin;
            int cmp = strcmp(tags->name[mid], name);
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

bool lyric_tags_insert(Tags *const restrict tags, const char *const restrict name, const char *const restrict value) {
    if (unlikely(tags == NULL || name == NULL || value == NULL)) {
        return false;
    }
    bool exact;
    const size_t position = _find_name(tags, name, &exact);
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
        lyric_free(tags->name[position]);
        lyric_free(tags->value[position]);
        tags->name[position] = new_name;
        tags->value[position] = new_value;
        return true;
    }
    if (unlikely(tags->size == tags->_malloc_size)) {
        size_t size = tags->_malloc_size;
        void *const name_array = lyric_extend_array(tags->name, sizeof(char *), &size);
        if (unlikely(name_array == NULL)) {
            return false;
        }
        tags->name = name_array;
        size = tags->_malloc_size;
        void *const value_array = lyric_extend_array(tags->value, sizeof(char *), &size);
        if (unlikely(value_array == NULL)) {
            return false;
        }
        tags->value = value_array;
        tags->_malloc_size = size;
    }
    memmove(tags->name + position + 1, tags->name + position, sizeof(char *) * (tags->size - position));
    tags->name[position] = lyric_strdup(name);
    if (unlikely(tags->name[position] == NULL)) {
        goto err1;
    }
    memmove(tags->value + position + 1, tags->value + position, sizeof(char *) * (tags->size - position));
    tags->value[position] = lyric_strdup(value);
    if (unlikely(tags->value[position] == NULL)) {
        goto err0;
    }
    ++tags->size;
    return true;
err1:
    memmove(tags->value + position, tags->value + position + 1, sizeof(char *) * (tags->size - position));
    lyric_free(tags->name[position]);
err0:
    memmove(tags->name + position, tags->name + position + 1, sizeof(char *) * (tags->size - position));
    return false;
}

void lyric_tags_remove(Tags *const restrict tags, char *const restrict name) {
    if (unlikely(name == NULL)) {
        return;
    }
    size_t begin = 0, end = tags->size - 1;
    while (begin < end) {
        size_t mid = (end - begin) / 2 + begin;
        int cmp = strcmp(tags->name[mid], name);
        if (cmp > 0) {
            begin = mid;
        } else if (cmp < 0) {
            end = mid;
        } else {
            lyric_free(tags->name[mid]);
            lyric_free(tags->value[mid]);
            memmove(tags->name + mid, tags->name + mid + 1, sizeof(char *) * (tags->size - mid));
            memmove(tags->value + mid, tags->value + mid + 1, sizeof(char *) * (tags->size - mid));
            --tags->size;
            return;
        }
    }
}

// vim:ts=4 sts=4 sw=4 et
