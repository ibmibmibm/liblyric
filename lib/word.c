#include "word.h"

bool lyric_word_create(Word *const restrict word) {
    if (unlikely(word == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_time_create(&word->time))) {
        goto err0;
    }
    word->string = NULL;
    return true;
err0:
    return false;
}

bool lyric_word_create_from_data(Word *const restrict word, Time *const restrict time, char *const restrict string) {
    if (unlikely(word == NULL || time == NULL || string == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_time_copy(&word->time, time))) {
        goto err0;
    }
    word->string = lyric_strdup(string);
    if (word->string == NULL) {
        goto err1;
    }
    return true;
err1:
    lyric_time_clean(&word->time);
err0:
    return false;
}

bool lyric_word_copy(Word *const restrict word, const Word *const restrict _word) {
    if (unlikely(word == NULL || _word == NULL)) {
        goto err0;
    }
    if (unlikely(word == _word)) {
        return true;
    }
    if (unlikely(!lyric_time_copy(&word->time, &_word->time))) {
        goto err0;
    }
    word->string = lyric_strdup(_word->string);
    if (unlikely(word->string == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_time_clean(&word->time);
err0:
    return false;
}

void lyric_word_clean(Word *const restrict word) {
    lyric_time_clean(&word->time);
    lyric_free(word->string);
}
// vim:ts=4 sts=4 sw=4 et
