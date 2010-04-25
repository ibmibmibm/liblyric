#ifndef __LINE_H__
#define __LINE_H__

#include "helpers.h"
#include "time.h"
#include "word.h"

struct _Line {
    Time time;
    Word *words;
    size_t word_size;
    size_t _malloc_word_size;
};
typedef struct _Line Line;

Line *lyric_line_new(void);
bool lyric_line_create(Line *const restrict line);
bool lyric_line_copy(Line* const restrict line, const Line *const restrict _line);
void lyric_line_clean(Line* const restrict line);

#endif // __LINE_H__
