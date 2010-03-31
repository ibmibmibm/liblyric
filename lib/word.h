#ifndef __WORD_H__
#define __WORD_H__

#include <stdio.h>
#include "time.h"

struct _Word {
    Time **times;
    char **strings;
    size_t size;
    size_t _malloc_size;
};
typedef struct _Word Word;

Word* lyric_word_new(void);
Word* lyric_word_new_from_file(FILE *file);
void lyric_word_save_to_file(const Word *word, FILE *file);
void lyric_word_delete(Word *word);

#endif // __WORD_H__
