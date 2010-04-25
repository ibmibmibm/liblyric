#include "line.h"

bool lyric_line_create(Line *const restrict line) {
    if (unlikely(!lyric_time_create(&line->time))) {
        goto err0;
    }
    line->word_size = 0;
    line->_malloc_word_size = 1;
    line->words = (Word*)lyric_alloc(sizeof(Word));
    if (unlikely(line->words == NULL)) {
        goto err1;
    }
    return true;
err1:
    lyric_time_clean(&line->time);
err0:
    return false;
}

Line *lyric_line_new(void) {
    Line *line = (Line*)lyric_alloc(sizeof(Line));
    if (unlikely(line == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_line_create(line))) {
        goto err1;
    }
    return line;
err1:
    lyric_free(line);
err0:
    return NULL;
}

bool lyric_line_copy(Line* const restrict line, const Line *const restrict _line) {
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
    line->words = (Word*)lyric_alloc(sizeof(Word) * size);
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
void lyric_line_clean(Line* const restrict line) {
    if (unlikely(line == NULL)) {
        return;
    }
    lyric_time_clean(&line->time);
    for (size_t i = 0; i < line->word_size; ++i) {
        lyric_word_clean(&line->words[i]);
    }
    lyric_free(line->words);
}
