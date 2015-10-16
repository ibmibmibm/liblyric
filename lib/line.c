#include "line.h"

bool lyric_line_create(Line *const restrict line) {
    if (unlikely(!lyric_time_create(&line->time))) {
        goto err0;
    }
    line->word_size = 0;
    line->_malloc_word_size = 1;
    line->words = (Word *)lyric_alloc(sizeof(Word));
    if (unlikely(line->words == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_time_clean(&line->time);
err0:
    return false;
}

bool lyric_line_copy(Line *const restrict line, const Line *const restrict _line) {
    if (unlikely(line == NULL || _line == NULL)) {
        goto err0;
    }
    if (unlikely(line == _line)) {
        return true;
    }
    if (unlikely(!lyric_time_copy(&line->time, &_line->time))) {
        goto err0;
    }
    const size_t size = _line->word_size;
    line->_malloc_word_size = size;
    line->words = (Word *)lyric_alloc(sizeof(Word) * size);
    if (unlikely(line->words == NULL)) {
        goto err1;
    }
    for (line->word_size = 0; line->word_size < size; ++line->word_size) {
        if (unlikely(!lyric_word_copy(&line->words[line->word_size], &_line->words[line->word_size]))) {
            goto err2;
        }
    }
    return true;
err2:
    for (size_t i = 0; i < line->word_size; ++i) {
        lyric_word_clean(&line->words[i]);
    }
err1:
    lyric_time_clean(&line->time);
err0:
    return false;
}

void lyric_line_clean(Line *const restrict line) {
    if (unlikely(line == NULL)) {
        return;
    }
    lyric_time_clean(&line->time);
    for (size_t i = 0; i < line->word_size; ++i) {
        lyric_word_clean(&line->words[i]);
    }
    lyric_free(line->words);
}

bool lyric_line_insert(Line *const restrict line, const size_t position, const Word *const restrict word) {
    if (unlikely(line->word_size < position || word == NULL)) {
        return false;
    }
    if (unlikely(line->word_size == line->_malloc_word_size)) {
        void *const array = lyric_extend_array(line->words, sizeof(Word), &line->_malloc_word_size);
        if (unlikely(array == NULL)) {
            return false;
        }
        line->words = array;
    }
    memmove(line->words + position + 1, line->words + position, sizeof(Word) * (line->word_size - position));
    if (unlikely(!lyric_word_copy(&line->words[position], word))) {
        memmove(line->words + position, line->words + position + 1, sizeof(Word) * (line->word_size - position));
        return false;
    }
    ++line->word_size;
    return true;
}

void lyric_line_remove(Line *const restrict line, const size_t position, Word *const restrict word) {
    if (unlikely(line->word_size <= position)) {
        return;
    }

    if (unlikely(word != NULL)) {
        memcpy(word, &line->words[position], sizeof(Word));
    }
    memmove(line->words + position, line->words + position + 1, sizeof(Word) * (line->word_size - position));
    --line->word_size;
}

bool lyric_line_push_back(Line *const restrict line, const Word *const restrict word) {
    return lyric_line_insert(line, line->word_size, word);
}

void lyric_line_pop_back(Line *const restrict line, Word *const restrict word) {
    lyric_line_remove(line, line->word_size - 1, word);
}
// vim:ts=4 sts=4 sw=4 et
