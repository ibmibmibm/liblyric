#ifndef __SENTENCE_H__
#define __SENTENCE_H__

#include <stdio.h>
#include "time.h"
#include "word.h"

struct _Time;
struct _Word;

struct _Sentence {
    struct _Time *time;
    struct _Word **word;
    size_t word_size;
    size_t _malloc_word_size;
};
typedef struct _Sentence Sentence;

Sentence* lyric_sentence_new(void);
Sentence* lyric_sentence_new_from_file(FILE *file);
void lyric_sentence_save_to_file(const Sentence *sentence, FILE *file);
void lyric_sentence_delete(Sentence *sentence);

#endif // __SENTENCE_H__
