#ifndef __WORD_H__
#define __WORD_H__

#include "helpers.h"
#include "time.h"

struct _Word {
    Time time;
    char *string;
};
typedef struct _Word Word;

bool lyric_word_copy(Word *const restrict word, const Word *const restrict _word);
void lyric_word_clean(Word *const restrict word);

#endif // __WORD_H__
