#include <json.h>
#include "parser.h"

static const char lyric_string_tags[] = {'t', 'a', 'g', 's'};
static const char lyric_string_singers[] = {'s', 'i', 'n', 'g', 'e', 'r', 's'};
static const char lyric_string_contents[] = {'c', 'o', 'n', 't', 'e', 'n', 't', 's'};

static const char *const string_of_errors[] = {
    [JSON_ERROR_NO_MEMORY] = "out of memory",
    [JSON_ERROR_BAD_CHAR] = "bad character",
    [JSON_ERROR_POP_EMPTY] = "stack empty",
    [JSON_ERROR_POP_UNEXPECTED_MODE] = "pop unexpected mode",
    [JSON_ERROR_NESTING_LIMIT] = "nesting limit",
    [JSON_ERROR_DATA_LIMIT] = "data limit",
    [JSON_ERROR_COMMENT_NOT_ALLOWED] = "comment not allowed by config",
    [JSON_ERROR_UNEXPECTED_CHAR] = "unexpected char",
    [JSON_ERROR_UNICODE_MISSING_LOW_SURROGATE] = "missing unicode low surrogate",
    [JSON_ERROR_UNICODE_UNEXPECTED_LOW_SURROGATE] = "unexpected unicode low surrogate",
    [JSON_ERROR_COMMA_OUT_OF_STRUCTURE] = "error comma out of structure",
    [JSON_ERROR_CALLBACK] = "error in a callback",
};

static int _json_parser_callback(void *const restrict userdata, const int type, const char *const restrict data, const uint32_t length) {
    struct _Parser *parser = userdata;
    // TODO: no mem detect
    switch (parser->status) {
        case lyric_parser_status_start: {
            if (type == JSON_ARRAY_BEGIN) {
            } else if (type == JSON_OBJECT_BEGIN) {
                if (parser->lyrics == NULL) {
                    parser->lyrics = lyric_lyric_new();
                    parser->size = 1;
                    parser->_malloc_size = 1;
                    parser->_d0 = parser->lyrics;
                } else {
                    if (parser->_malloc_size == parser->size) {
                        struct _Lyric *array = lyric_extend_array(parser->lyrics, sizeof(struct _Lyric), &parser->_malloc_size);
                        if (array == NULL)
                            return JSON_ERROR_NO_MEMORY;
                        parser->lyrics = array;
                    }
                    if (!lyric_lyric_create(&parser->lyrics[parser->size]))
                        return JSON_ERROR_NO_MEMORY;
                    ++parser->size;
                    parser->_d0 = &parser->lyrics[parser->size - 1];
                }
                parser->status = lyric_parser_status_lyric;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric: {
            if (type == JSON_KEY) {
                if (length == sizeof(lyric_string_tags) && lyric_strncasecmp(data, lyric_string_tags, sizeof(lyric_string_tags)) == 0) {
                    parser->_d1.t = &parser->_d0->tags;
                    parser->status = lyric_parser_status_lyric_tags;
                } else if (length == sizeof(lyric_string_singers) && lyric_strncasecmp(data, lyric_string_singers, sizeof(lyric_string_singers)) == 0) {
                    parser->status = lyric_parser_status_lyric_singers;
                } else {
                    return JSON_ERROR_CALLBACK;
                }
            } else if (type == JSON_OBJECT_END) {
                parser->status = lyric_parser_status_finish;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_tags: {
            if (type == JSON_OBJECT_BEGIN) {
                parser->status = lyric_parser_status_lyric_tags_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_tags_name: {
            if (type == JSON_KEY) {
                parser->_d2.k = lyric_strndup(data, length);
                parser->status = lyric_parser_status_lyric_tags_value;
            } else if (type == JSON_OBJECT_END) {
                parser->status = lyric_parser_status_lyric;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_tags_value: {
            if (type == JSON_STRING) {
                char *value = lyric_strndup(data, length);
                bool result = lyric_tags_insert(parser->_d1.t, parser->_d2.k, value);
                lyric_free(value);
                lyric_free(parser->_d2.k);
                if (!result)
                    return JSON_ERROR_NO_MEMORY;
                parser->status = lyric_parser_status_lyric_tags_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singers: {
            if (type == JSON_ARRAY_BEGIN) {
            } else if (type == JSON_ARRAY_END) {
                parser->status = lyric_parser_status_lyric;
            } else if (type == JSON_OBJECT_BEGIN) {
                Singer singer;
                lyric_singer_create(&singer);
                bool result = lyric_lyric_push_back(parser->_d0, &singer);
                lyric_singer_clean(&singer);
                if (!result)
                    return JSON_ERROR_NO_MEMORY;
                parser->_d1.s = &parser->_d0->singers[parser->_d0->singer_size - 1];
                parser->status = lyric_parser_status_lyric_singer;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer: {
            if (type == JSON_KEY) {
                if (length == sizeof(lyric_string_tags) && lyric_strncasecmp(data, lyric_string_tags, sizeof(lyric_string_tags)) == 0) {
                    parser->_d2.t = &parser->_d1.s->tags;
                    parser->status = lyric_parser_status_lyric_singer_tags;
                } else if (length == sizeof(lyric_string_contents) && lyric_strncasecmp(data, lyric_string_contents, sizeof(lyric_string_contents)) == 0) {
                    parser->status = lyric_parser_status_lyric_singer_contents;
                } else {
                    return JSON_ERROR_CALLBACK;
                }
            } else if (type == JSON_OBJECT_END) {
                parser->status = lyric_parser_status_lyric_singers;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_tags: {
            if (type == JSON_OBJECT_BEGIN) {
                parser->status = lyric_parser_status_lyric_singer_tags_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_tags_name: {
            if (type == JSON_KEY) {
                parser->_d3.k = lyric_strndup(data, length);
                parser->status = lyric_parser_status_lyric_singer_tags_value;
            } else if (type == JSON_OBJECT_END) {
                parser->status = lyric_parser_status_lyric_singer;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_tags_value: {
            if (type == JSON_STRING) {
                char *value = lyric_strndup(data, length);
                bool result = lyric_tags_insert(parser->_d2.t, parser->_d3.k, value);
                lyric_free(value);
                lyric_free(parser->_d3.k);
                if (!result)
                    return JSON_ERROR_NO_MEMORY;
                parser->status = lyric_parser_status_lyric_singer_tags_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_contents: {
            if (type == JSON_ARRAY_BEGIN) {
                parser->status = lyric_parser_status_lyric_singer_contents_line;
            } else if (type == JSON_ARRAY_END) {
                parser->status = lyric_parser_status_lyric_singer;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_contents_line: {
            if (type == JSON_ARRAY_BEGIN) {
                parser->status = lyric_parser_status_lyric_singer_contents_line_offset;
            } else if (type == JSON_ARRAY_END) {
                parser->status = lyric_parser_status_lyric_singer;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_contents_line_offset: {
            if (type == JSON_STRING) {
                // TODO: no mem detect
                Line line;
                lyric_line_create(&line);
                bool result = lyric_time_create_from_string(&line.time, data, length);
                if (unlikely(!result)) {
                    lyric_line_clean(&line);
                    return JSON_ERROR_CALLBACK;
                }
                lyric_singer_push_back(parser->_d1.s, &line);
                lyric_line_clean(&line);
                parser->_d2.l = &parser->_d1.s->lines[parser->_d1.s->line_size - 1];
                parser->status = lyric_parser_status_lyric_singer_contents_line_word;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_contents_line_word: {
            if (type == JSON_STRING) {
                parser->_d3.k = lyric_strndup(data, length);
                parser->status = lyric_parser_status_lyric_singer_contents_line_time;
            } else if (type == JSON_ARRAY_END) {
                parser->status = lyric_parser_status_lyric_singer_contents_line;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_contents_line_time: {
            if (type == JSON_INT) {
                Time time;
                lyric_time_create_from_literal(&time, data, length);
                Word word;
                lyric_word_create_from_data(&word, &time, parser->_d3.k);
                lyric_free(parser->_d3.k);
                lyric_time_clean(&time);
                lyric_line_push_back(parser->_d2.l, &word);
                lyric_word_clean(&word);
                parser->status = lyric_parser_status_lyric_singer_contents_line_word;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_finish: {
            if (type == JSON_OBJECT_BEGIN) {
                if (parser->lyrics == NULL) {
                    parser->lyrics = lyric_lyric_new();
                    parser->size = 1;
                    parser->_malloc_size = 1;
                    parser->_d0 = parser->lyrics;
                } else {
                    if (parser->_malloc_size == parser->size) {
                        struct _Lyric *array = lyric_extend_array(parser->lyrics, sizeof(struct _Lyric), &parser->_malloc_size);
                        if (array == NULL)
                            return JSON_ERROR_NO_MEMORY;
                        parser->lyrics = array;
                    }
                    if (!lyric_lyric_create(&parser->lyrics[parser->size]))
                        return JSON_ERROR_NO_MEMORY;
                    ++parser->size;
                    parser->_d0 = &parser->lyrics[parser->size - 1];
                }
                parser->status = lyric_parser_status_lyric;
            } else if (type == JSON_OBJECT_END) {
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        default:
            return JSON_ERROR_CALLBACK;
    }
}

static int _do_process_file(json_parser *const restrict parser, size_t *restrict lines, size_t *restrict col, FILE *const restrict file) {
    int ret;

    fseek(file, 0, SEEK_SET);
    *lines = 1;
    *col = 0;
    while (true) {
        char buffer[LIBJSON_DEFAULT_BUFFER_SIZE];
        int32_t read = fread(buffer, 1, LIBJSON_DEFAULT_BUFFER_SIZE, file);
        if (read <= 0)
            break;
        uint32_t processed;
        ret = json_parser_string(parser, buffer, read, &processed);
        for (uint32_t i = 0; i < processed; i++) {
            if (buffer[i] == '\n') {
                *col = 0;
                ++*lines;
            } else {
                ++*col;
            }
        }
        if (ret)
            break;
    }
    return ret;
}

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict file) {
    if (unlikely(parser == NULL || file == NULL))
        return false;

    json_config config = {
        .max_nesting = 0,
        .max_data = 0,
        .allow_c_comments = 1,
        .allow_yaml_comments = 1,
    };
    json_parser _parser;
    int ret = json_parser_init(&_parser, &config, _json_parser_callback, parser);
    if (ret != 0) {
        fprintf(stderr, "error: initializing parser failed (code=%d): %s\n", ret, string_of_errors[ret]);
        return false;
    }
    ret = _do_process_file(&_parser, &parser->lines, &parser->col, file);
    if (ret != 0) {
        fprintf(stderr, "line %zd, col %zd: [code=%d] %s\n", parser->lines, parser->col, ret, string_of_errors[ret]);
        return false;
    }
    if (json_parser_is_done(&_parser) == 0 || parser->status != lyric_parser_status_finish) {
        return false;
    }
    return true;
}

Parser *lyric_parser_new(void) {
    Parser *parser = lyric_alloc(sizeof(Parser));
    if (unlikely(parser == NULL))
        return NULL;
    parser->status = lyric_parser_status_start;
    parser->error = lyric_parser_noerror;
    parser->lines = parser->col = 0;
    parser->lyrics = NULL;
    parser->size = 0;
    return parser;
}

Parser *lyric_parser_new_from_file(FILE *const restrict file) {
    if (unlikely(file == NULL))
        return NULL;
    Parser *parser = lyric_parser_new();
    if (unlikely(parser == NULL))
        return NULL;
    if (unlikely(!lyric_parser_from_file(parser, file))) {
        lyric_free(parser);
        return NULL;
    }
    return parser;
}

Lyric *lyric_read_file(FILE *const restrict file) {
    Parser *parser = lyric_parser_new_from_file(file);
    if (unlikely(parser == NULL))
        return NULL;
    Lyric *result = lyric_lyric_new_copy(parser->lyrics);
    lyric_parser_delete(parser);
    return result;
}

void lyric_write_file(const Lyric *const restrict lyric, FILE *const restrict file) {
    if (unlikely(lyric == NULL || file == NULL))
        return;
    fprintf(file, "{\n");
    fprintf(file, "    \"tags\": {\n");
    for (size_t i = 0; i < lyric->tags.size; ++i) {
        fprintf(file, "        \"%s\": \"%s\",\n", lyric->tags.name[i], lyric->tags.value[i]);
    }
    fprintf(file, "    },\n");
    fprintf(file, "    \"singers\": [\n");
    for (size_t i = 0; i < lyric->singer_size; ++i) {
        fprintf(file, "        {\n");
        fprintf(file, "            \"tags\": {\n");
        for (size_t j = 0; j < lyric->singers[i].tags.size; ++j) {
            fprintf(file, "                \"%s\": \"%s\",\n", lyric->singers[i].tags.name[j], lyric->singers[i].tags.value[j]);
        }
        fprintf(file, "            },\n");
        fprintf(file, "            \"contents\": [\n");
        for (size_t j = 0; j < lyric->singers[i].line_size; ++j) {
            char *const time_string = lyric_time_to_new_string(&lyric->singers[i].lines[j].time);
            fprintf(file, "                [\"%s\"", time_string);
            lyric_free(time_string);
            for (size_t k = 0; k < lyric->singers[i].lines[j].word_size; ++k) {
                fprintf(file, ", \"%s\", %zd", lyric->singers[i].lines[j].words[k].string, lyric_time_to_literal(&lyric->singers[i].lines[j].words[k].time));
            }
            if (j + 1 < lyric->singers[i].line_size) {
                fprintf(file, "],\n");
            } else {
                fprintf(file, "]\n");
            }
        }
        fprintf(file, "            ]\n");
        if (i + 1 < lyric->singer_size) {
            fprintf(file, "        },\n");
        } else {
            fprintf(file, "        }\n");
        }
    }
    fprintf(file, "    ]\n");
    fprintf(file, "}\n");
}

void lyric_parser_clean(Parser *const restrict parser) {
    if (unlikely(parser == NULL))
        return;
    for (size_t i = 0; i < parser->size; ++i)
        lyric_lyric_clean(&parser->lyrics[i]);
    lyric_free(parser->lyrics);
}

void lyric_parser_delete(Parser *const restrict parser) {
    if (unlikely(parser == NULL))
        return;
    lyric_parser_clean(parser);
    lyric_free(parser);
}
// vim:ts=4 sts=4 sw=4 et
