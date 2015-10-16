#include "buffer.h"

Buffer *lyric_buffer_new(void) {
    Buffer *buffer = (Buffer *)lyric_alloc(sizeof(Buffer));
    if (unlikely(buffer == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_buffer_create(buffer))) {
        goto err1;
    }
    return buffer;
err1:
    lyric_free(buffer);
err0:
    return NULL;
}

void lyric_buffer_delete(Buffer *const restrict buffer) {
    lyric_buffer_clean(buffer);
    lyric_free(buffer);
}

bool lyric_buffer_create(Buffer *const restrict buffer) {
    buffer->data = (char *)lyric_alloc(sizeof(char));
    if (unlikely(buffer->data == NULL)) {
        return false;
    }
    buffer->size = 0;
    buffer->_malloc_size = 1;
    return true;
}

void lyric_buffer_clean(Buffer *const restrict buffer) {
    lyric_free(buffer->data);
}

bool lyric_buffer_putchar(Buffer *const restrict buffer, char c) {
    if (unlikely(buffer->size == buffer->_malloc_size)) {
        if (unlikely(!lyric_extend_array((void**)&buffer->data, sizeof(char), &buffer->_malloc_size))) {
            return false;
        }
    }
    buffer->data[buffer->size++] = c;
    return true;
}

bool lyric_buffer_puts(Buffer *const restrict buffer, const char *const restrict s, size_t size) {
    while (unlikely(buffer->size + size >= buffer->_malloc_size)) {
        if (unlikely(!lyric_extend_array((void**)&buffer->data, sizeof(char), &buffer->_malloc_size))) {
            return false;
        }
    }
    memcpy(&buffer->data[buffer->size], s, size);
    buffer->size += size;
    return true;
}

void lyric_buffer_clear(Buffer *const restrict buffer) {
    buffer->size = 0;
}
