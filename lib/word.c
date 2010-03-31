#include <stdlib.h>
#include "word.h"

Word* lyric_word_new(void) {
    Word *word = (Word*)calloc(sizeof(Word), 1);
    if (word == NULL)
        return NULL;
    word->size = 0;
    word->_malloc_size = 1;
    word->times = (Time**)calloc(sizeof(Time*), 1);
    if (word->times == NULL) {
        lyric_word_delete(word);
        return NULL;
    }
    word->strings = (char**)calloc(sizeof(char*), 1);
    if (word->strings == NULL) {
        lyric_word_delete(word);
        return NULL;
    }
    return word;
}

Word* lyric_word_new_from_file(FILE *file) {
    Word *word = (Word*)calloc(sizeof(Word), 1);
    if (word == NULL)
        return NULL;
    word->size = 0;
    word->_malloc_size = 1;
    word->times = (Time**)calloc(sizeof(Time*), 1);
    if (word->times == NULL) {
        lyric_word_delete(word);
        return NULL;
    }
    word->strings = (char**)calloc(sizeof(char*), 1);
    if (word->strings == NULL) {
        lyric_word_delete(word);
        return NULL;
    }
    return word;
}

void lyric_word_save_to_file(const Word *word, FILE *file) {
    size_t i;
    if (word == NULL)
        return;
    fputc('[', file);
    lyric_time_save_to_file_shortform(word->times[0], file);
    for (i = 1; i < word->size; ++i) {
        fputc(',', file);
        lyric_time_save_to_file_shortform(word->times[i], file);
    }
    fputc(']', file);
    fputs(word->strings[0], file);
    for (i = 1; i < word->size; ++i) {
        fputc(',', file);
        fputs(word->strings[i], file);
    }
    fputc('\n', file);
}

void lyric_word_delete(Word *word) {
    size_t i;
    if (word == NULL)
        return;
    if (word->times) {
        for (i = 0; i < word->size; ++i) {
            lyric_time_delete(word->times[i]);
        }
        free(word->times);
    }
    if (word->strings) {
        for (i = 0; i < word->size; ++i) {
            free(word->strings[i]);
        }
        free(word->strings);
    }
    free(word);
}
