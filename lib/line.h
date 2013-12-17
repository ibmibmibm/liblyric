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

bool lyric_line_create(Line *const restrict line);
bool lyric_line_copy(Line* const restrict line, const Line *const restrict _line);
void lyric_line_clean(Line* const restrict line);

bool lyric_line_insert(Line *const restrict line, const size_t position, const Word *const restrict word);
void lyric_line_remove(Line *const restrict line, const size_t position, Word *const restrict word);
bool lyric_line_push_back(Line *const restrict line, const Word *const restrict word);
void lyric_line_pop_back(Line *const restrict line, Word *const restrict word);

#endif // __LINE_H__
// vim:ts=4 sts=4 sw=4 et
