#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "helpers.h"
#include "lyric.h"

struct _Buffer {
    char *data;
    size_t size;
    size_t _malloc_size;
};
typedef struct _Buffer Buffer;

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
Buffer *lyric_buffer_new(void);

void lyric_buffer_delete(Buffer *const restrict buffer);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_buffer_create(Buffer *const restrict buffer);

void lyric_buffer_clean(Buffer *const restrict buffer);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_buffer_putchar(Buffer *const restrict buffer, char c);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_buffer_puts(Buffer *const restrict buffer, const char *const restrict s, size_t size);

void lyric_buffer_clear(Buffer *const restrict buffer);

#endif // __BUFFER_H__
// vim:ts=4 sts=4 sw=4 et
