#include "parser.h"

static inline void lyric_parser_switch(Parser *const restrict parser);

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict file) {
    if (unlikely(parser == NULL || file == NULL))
        return false;
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);
    parser->buffer = lyric_alloc(size);
    parser->length = 0;
    while (parser->length < size) {
        const size_t remain = size - parser->length;
        const size_t to_read = lyric_min(4096, remain);
        const size_t readed = fread(parser->buffer + parser->length, 1, to_read, file);
        if (unlikely(readed == 0)) {
            if (feof(file) || ferror(file)) {
                lyric_free(parser->buffer);
                return false;
            }
        }
        parser->length += readed;
    }
    parser->lyric = lyric_lyric_new();
    if (unlikely(parser->lyric == NULL)) {
        lyric_free(parser->buffer);
        return false;
    }
    parser->cursor = parser->buffer;
    parser->buffer_end = parser->buffer + parser->length;
    parser->line_number = 1;
    parser->char_number = 1;
    parser->status = lyric_parser_status_finish;
    parser->error = lyric_parser_noerror;
    return true;
}

Parser *lyric_parser_new_from_file(FILE *const restrict file) {
    if (unlikely(file == NULL))
        return NULL;
    Parser *parser = lyric_alloc(sizeof(Parser));
    if (unlikely(parser == NULL))
        return NULL;
    if (unlikely(!lyric_parser_from_file(parser, file))) {
        lyric_free(parser);
        return NULL;
    }
    return parser;
}

static bool lyric_parser_advance(Parser *const restrict parser) {
    if (*parser->cursor == '\n') {
        ++parser->line_number;
        parser->char_number = 1;
    } else {
        ++parser->char_number;
    }
    ++parser->cursor;
    return parser->cursor != parser->buffer_end;
}

static void lyric_parser_lyric_start(Parser *const restrict parser) {
    if (*parser->cursor == '{' && lyric_parser_advance(parser)) {
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_status_tag_string;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_lyric_start;
        return;
    }
}

static void lyric_parser_lyric_finish(Parser *const restrict parser) {
    if (*parser->cursor == '}') {
        if (!lyric_parser_advance(parser)) {
            parser->status = lyric_parser_status_finish;
            return;
        }
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_status_finish;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_lyric_finish;
    }
}

static void lyric_parser_whitespace(Parser *const restrict parser) {
    while (isspace(*parser->cursor)) {
        if (!lyric_parser_advance(parser)) {
            parser->status = parser->next_status;
            return;
        }
    }
    parser->status = parser->next_status;
    return lyric_parser_switch(parser);
}

static void lyric_parser_tag_string(Parser *const restrict parser) {
    const char TAGNAME[] = "\"tag\"";
    const size_t TAGSIZE = sizeof(TAGNAME) - 1;
    if (parser->cursor + TAGSIZE >= parser->buffer_end || lyric_strncasecmp(TAGNAME, parser->cursor, TAGSIZE) != 0) {
        parser->error = lyric_parser_error_tag_string;
        return;
    }
    parser->cursor += TAGSIZE;
    if (*parser->cursor == '\n') {
        ++parser->line_number;
        parser->char_number = 0;
    } else {
        parser->char_number += TAGSIZE;
    }
    parser->next_status = lyric_parser_status_tag_colon;
    parser->status = lyric_parser_status_whitespace;
    return lyric_parser_switch(parser);
}

static void lyric_parser_tag_colon(Parser *const restrict parser) {
    if (*parser->cursor == ':' && lyric_parser_advance(parser)) {
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_status_tag_start;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_tag_colon;
        return;
    }
}

static void lyric_parser_tag_start(Parser *const restrict parser) {
    if (*parser->cursor == '{' && lyric_parser_advance(parser)) {
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_status_tag_name;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_tag_start;
        return;
    }
}

static void lyric_parser_tag_name(Parser *const restrict parser) {
    if (*parser->cursor == '"' && lyric_parser_advance(parser)) {
        parser->status = lyric_parser_status_string;
        parser->next_status = lyric_parser_status_tag_name_colon;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_tag_name;
        return;
    }
}

static void lyric_parser_tag_name_colon(Parser *const restrict parser) {
    if (*parser->cursor == ':' && lyric_parser_advance(parser)) {
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_status_tag_value;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_tag_name_colon;
        return;
    }
}

static void lyric_parser_tag_value(Parser *const restrict parser) {
    if (*parser->cursor == '"' && lyric_parser_advance(parser)) {
        parser->status = lyric_parser_status_string;
        parser->next_status = lyric_parser_status_tag_finish;
        return lyric_parser_switch(parser);
    } else {
        parser->error = lyric_parser_error_tag_name;
        return;
    }
}

static void lyric_parser_tag_finish(Parser *const restrict parser) {
    if (*parser->cursor == ',') {
        if (!lyric_parser_advance(parser)) {
            goto err;
        }
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_status_tag_name;
        return lyric_parser_switch(parser);
    } else if (*parser->cursor == '}') {
        if (!lyric_parser_advance(parser)) {
            goto err;
        }
        parser->status = lyric_parser_status_whitespace;
        parser->next_status = lyric_parser_lyric_finish;
        return lyric_parser_switch(parser);
    }
err:
    parser->error = lyric_parser_error_singer_string;
    return;
}

static void lyric_parser_singer_string(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_singer_start(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_singer_name(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_singer_gender(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_singer_content(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_singer_stop(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_string_begin(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_string(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_string_escape(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_string_unicode(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_number(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_finish(Parser *const restrict parser) {
    return lyric_parser_switch(parser);
}

static void lyric_parser_switch(Parser *const restrict parser) {
    switch (parser->status) {
        case lyric_parser_status_start:
            return lyric_parser_start(parser);
        case lyric_parser_status_whitespace:
            return lyric_parser_whitespace(parser);
        case lyric_parser_status_lyric_start:
            return lyric_parser_lyric_start(parser);
        case lyric_parser_status_lyric_finish:
            return lyric_parser_lyric_finish(parser);
        case lyric_parser_status_tag_string:
            return lyric_parser_tag_string(parser);
        case lyric_parser_status_tag_colon:
            return lyric_parser_tag_colon(parser);
        case lyric_parser_status_tag_start:
            return lyric_parser_tag_start(parser);
        case lyric_parser_status_tag_name:
            return lyric_parser_tag_name(parser);
        case lyric_parser_status_tag_value:
            return lyric_parser_tag_value(parser);
        case lyric_parser_status_tag_finish:
            return lyric_parser_tag_finish(parser);
        case lyric_parser_status_singer_string:
            return lyric_parser_singer_string(parser);
        case lyric_parser_status_singer_start:
            return lyric_parser_singer_start(parser);
        case lyric_parser_status_singer_name:
            return lyric_parser_singer_name(parser);
        case lyric_parser_status_singer_gender:
            return lyric_parser_singer_gender(parser);
        case lyric_parser_status_singer_content:
            return lyric_parser_singer_content(parser);
        case lyric_parser_status_singer_stop:
            return lyric_parser_singer_stop(parser);
        case lyric_parser_status_string_begin:
            return lyric_parser_string_begin(parser);
        case lyric_parser_status_string:
            return lyric_parser_string(parser);
        case lyric_parser_status_string_escape:
            return lyric_parser_string_escape(parser);
        case lyric_parser_status_string_unicode:
            return lyric_parser_string_unicode(parser);
        case lyric_parser_status_number:
            return lyric_parser_number(parser);
        case lyric_parser_status_finish:
            return lyric_parser_finish(parser);
    }
}

void lyric_parser_start(Parser *const restrict parser) {
    if (unlikely(parser == NULL)) {
        return;
    }
    parser->status = lyric_parser_status_whitespace;
    parser->next_status = lyric_parser_status_lyric_start;
    return lyric_parser_switch(parser);
}

Lyric *lyric_read_file(FILE *const restrict file) {
    Parser *parser = lyric_parser_new_from_file(file);
    if (unlikely(parser == NULL))
        return NULL;
    lyric_parser_start(parser);
    if (parser->status == lyric_parser_status_finish) {
        Lyric *result = parser->lyric;
        lyric_free(parser->buffer);
        lyric_free(parser);
        return result;
    } else {
        lyric_free(parser->lyric);
        lyric_free(parser->buffer);
        lyric_free(parser);
        return NULL;
    }
}

void lyric_write_file(const Lyric *const restrict lyric, FILE *const restrict file) {
}
