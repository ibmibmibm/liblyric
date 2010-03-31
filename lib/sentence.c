#include <stdlib.h>
#include "sentence.h"

Sentence* lyric_sentence_new(void) {
    Sentence *sentence = (Sentence*)calloc(sizeof(Sentence), 1);
    if (sentence == NULL)
        return NULL;
    sentence->word_size = 0;
    sentence->_malloc_word_size = 1;
    sentence->word = (Word**)malloc(sizeof(Word*));
    if (sentence->word == NULL) {
        free(sentence);
        return NULL;
    }
    return sentence;
}

Sentence* lyric_sentence_new_from_file(FILE *file) {
    Sentence *sentence = (Sentence*)calloc(sizeof(Sentence), 1);
    if (sentence == NULL)
        return NULL;
    sentence->word_size = 0;
    sentence->_malloc_word_size = 1;
    sentence->word = (Word**)malloc(sizeof(Word*));
    if (sentence->word == NULL) {
        free(sentence);
        return NULL;
    }
    return sentence;
}

void lyric_sentence_save_to_file(const Sentence *sentence, FILE *file) {
    size_t i;
    if (sentence == NULL)
        return;
    lyric_time_save_to_file_longform(sentence->time, file);
    fputc(' ', file);
    for (i = 0; i < sentence->word_size; ++i)
        lyric_word_save_to_file(sentence->word[i], file);
    fputc('\n', file);
}

void lyric_sentence_delete(Sentence *sentence) {
    size_t i;
    if (sentence == NULL)
        return;
    if (sentence->word) {
        for (i = 0; i < sentence->word_size; ++i)
            lyric_word_delete(sentence->word[i]);
        free(sentence->word);
    }
    free(sentence);
}

