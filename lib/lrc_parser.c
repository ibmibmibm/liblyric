#include "line.h"
#include "lrc_parser.h"

LRCParser *lyric_lrc_parser_new(void) {
    LRCParser *parser = lyric_alloc(sizeof(LRCParser));
    if (unlikely(parser == NULL)) {
        return NULL;
    }
    parser->lyric = NULL;
    return parser;
}

LRCParser *lyric_lrc_parser_new_from_file(FILE *const restrict file) {
    if (unlikely(file == NULL)) {
        return NULL;
    }
    LRCParser *parser = lyric_lrc_parser_new();
    if (unlikely(parser == NULL)) {
        return NULL;
    }
    if (unlikely(!lyric_lrc_parser_from_file(parser, file))) {
        lyric_free(parser);
        return NULL;
    }
    return parser;
}

Lyric *lyric_read_lrc_file(FILE *const restrict file) {
    LRCParser *parser = lyric_lrc_parser_new_from_file(file);
    if (unlikely(parser == NULL)) {
        return NULL;
    }
    Lyric *result = lyric_lyric_new_copy(parser->lyric);
    lyric_lrc_parser_delete(parser);
    return result;
}

void lyric_lrc_parser_clean(LRCParser *const restrict parser) {
    if (unlikely(parser == NULL)) {
        return;
    }
    if (parser->lyric) {
        lyric_lyric_clean(parser->lyric);
        parser->lyric = NULL;
    }
}

void lyric_lrc_parser_delete(LRCParser *const restrict parser) {
    if (unlikely(parser == NULL)) {
        return;
    }
    lyric_lrc_parser_clean(parser);
    lyric_free(parser);
}

bool lyric_lrc_parser_from_file(LRCParser *const restrict parser, FILE *const restrict file) {
    bool result = false;
    if (unlikely(parser == NULL || file == NULL)) {
        goto err0;
    }
    fseek(file, 0, SEEK_SET);
    parser->status = lyric_lrcparser_status_start;
    parser->error = lyric_lrcparser_noerror;
    parser->lines = 1;
    parser->col = 0;
    parser->lyric = lyric_lyric_new();
    if (unlikely(!parser->lyric)) {
        goto err1;
    }
    parser->_buffer = lyric_buffer_new();
    if (unlikely(!parser->_buffer)) {
        goto err2;
    }
    parser->_singer = lyric_singer_new();
    if (unlikely(!parser->_singer)) {
        goto err3;
    }
    parser->_line = lyric_line_new();
    if (unlikely(!parser->_line)) {
        goto err4;
    }
    bool finished = false;
    while (!finished && parser->error == lyric_lrcparser_noerror) {
        char buffer[LIBLYRIC_DEFAULT_BUFFER_SIZE];
        int32_t read = fread(buffer, 1, sizeof(buffer), file);
        printf("read=%d\n",read);
        if (unlikely(read <= 0)) {
            // simulation the last char as a newline
            finished = true;
            buffer[0] = '\n';
            read = 1;
        }
        for (int32_t cursor = 0; cursor < read; ++cursor) {
            const char c = buffer[cursor];
            if (unlikely(c == '\n')) {
                parser->col = 0;
                ++parser->lines;
            } else {
                ++parser->col;
            }
            printf("%zu:%zu %c %u\n", parser->lines, parser->col, c, parser->status);
            switch (parser->status) {
                case lyric_lrcparser_status_start: {
                    if (likely(c == '[')) {
                        parser->status = lyric_lrcparser_status_tag;
                    } else if (likely(c != '\n')) {
                        parser->status = lyric_lrcparser_status_lyric;
                    }
                    break;
                }
                case lyric_lrcparser_status_tag: {
                    if (likely(isdigit(c))) {
                        parser->status = lyric_lrcparser_status_time;
                        if (unlikely(!lyric_buffer_putchar(parser->_buffer, c))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                    } else if (unlikely(c == '\n')) {
                        parser->error = lyric_lrcparser_error_unexcept_char;
                        break;
                    } else {
                        parser->status = lyric_lrcparser_status_tag_name;
                        if (unlikely(!lyric_buffer_putchar(parser->_buffer, c))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                    }
                    break;
                }
                case lyric_lrcparser_status_tag_name: {
                    if (unlikely(c == ':')) {
                        parser->_n = lyric_strndup(parser->_buffer->data, parser->_buffer->size);
                        if (unlikely(!parser->_n)) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                        lyric_buffer_clear(parser->_buffer);
                        parser->status = lyric_lrcparser_status_tag_value;
                    } else if (unlikely(c == ']' || c == '\n')) {
                        parser->error = lyric_lrcparser_error_unexcept_char;
                        break;
                    } else {
                        if (unlikely(!lyric_buffer_putchar(parser->_buffer, c))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                    }
                    break;
                }
                case lyric_lrcparser_status_tag_value: {
                    if (unlikely(c == ']')) {
                        char *const v = lyric_strndup(parser->_buffer->data, parser->_buffer->size);
                        if (unlikely(!v)) {
                            lyric_free(parser->_n);
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                        lyric_buffer_clear(parser->_buffer);
                        if (unlikely(!lyric_tags_insert(&parser->lyric->tags, parser->_n, v))) {
                            lyric_free(v);
                            lyric_free(parser->_n);
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                        lyric_free(v);
                        lyric_free(parser->_n);
                        parser->status = lyric_lrcparser_status_comment;
                    } else if (unlikely(c == '\n')) {
                        lyric_free(parser->_n);
                        parser->error = lyric_lrcparser_error_unexcept_char;
                        break;
                    } else {
                        if (unlikely(!lyric_buffer_putchar(parser->_buffer, c))) {
                            lyric_free(parser->_n);
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                    }
                    break;
                }
                case lyric_lrcparser_status_time: {
                    if (unlikely(c == ']')) {
                        if (unlikely(!lyric_time_create_from_string(&parser->_line->time, parser->_buffer->data, parser->_buffer->size))) {
                            parser->error = lyric_lrcparser_error_unexcept_char;
                        }
                        lyric_buffer_clear(parser->_buffer);
                        parser->status = lyric_lrcparser_status_lyric;
                    } else if (likely(isdigit(c) || c == ':' || c == '.')) {
                        if (unlikely(!lyric_buffer_putchar(parser->_buffer, c))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                        }
                    } else {
                        parser->error = lyric_lrcparser_error_unexcept_char;
                        break;
                    }
                    break;
                }
                case lyric_lrcparser_status_lyric: {
                    if (unlikely(c == '\n')) {
                        Word w;
                        if (unlikely(!lyric_word_create(&w))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                        w.string = lyric_strndup(parser->_buffer->data, parser->_buffer->size);
                        if (unlikely(!w.string ||
                                     !lyric_line_push_back(parser->_line, &w) ||
                                     !lyric_singer_push_back(parser->_singer, parser->_line))) {
                            lyric_word_clean(&w);
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                        lyric_word_clean(&w);
                        lyric_line_clean(parser->_line);
                        if (unlikely(!lyric_line_create(parser->_line))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                        lyric_buffer_clear(parser->_buffer);
                        parser->status = lyric_lrcparser_status_start;
                    } else {
                        if (unlikely(!lyric_buffer_putchar(parser->_buffer, c))) {
                            parser->error = lyric_lrcparser_error_no_memory;
                            break;
                        }
                    }
                    break;
                }
                case lyric_lrcparser_status_comment: {
                    if (unlikely(c == '\n')) {
                        parser->status = lyric_lrcparser_status_start;
                    }
                    break;
                }
                default: {
                    parser->error = lyric_lrcparser_error_unknown;
                    break;
                }
            }
            if (unlikely(parser->error != lyric_lrcparser_noerror)) {
                break;
            }
        }
    }
    parser->status = lyric_lrcparser_status_finish;
    if (unlikely(!lyric_lyric_push_back(parser->lyric, parser->_singer))) {
        parser->error = lyric_lrcparser_error_no_memory;
        goto err5;
    }

    result = true;

err5:
    lyric_free(parser->_line);
err4:
    lyric_free(parser->_singer);
err3:
    lyric_free(parser->_buffer);
    if (!result) {
err2:
        lyric_free(parser->lyric);
err1:
        parser->error = lyric_lrcparser_error_no_memory;
        parser->status = lyric_lrcparser_status_finish;
    }
err0:
    return result && (parser->error == lyric_lrcparser_noerror);
}

// vim:ts=4 sts=4 sw=4 et
