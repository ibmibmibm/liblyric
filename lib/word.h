#ifndef __WORD_H__
#define __WORD_H__

#include "helpers.h"
#include "time.h"

struct _Word {
    Time time;
    char *string;
};
typedef struct _Word Word;

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_word_create(Word *const restrict word);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_word_create_from_data(Word *const restrict word, Time *const restrict time, char *const restrict string);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_word_copy(Word *const restrict word, const Word *const restrict _word);

void lyric_word_clean(Word *const restrict word);

#endif // __WORD_H__
// vim:ts=4 sts=4 sw=4 et
