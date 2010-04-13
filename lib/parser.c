#include <json.h>
#include "parser.h"

enum json_type {
    JSON_TYPE_NULL,
    JSON_TYPE_INT,
    JSON_TYPE_DOUBLE,
    JSON_TYPE_BOOL,
    JSON_TYPE_STRING,
    JSON_TYPE_ARRAY,
    JSON_TYPE_OBJECT,
};

struct json_val_element {
    char *key;
    uint32_t key_length;
    struct json_val *val;
};

typedef struct json_val {
    json_type type;
    int length;
    union {
        char *data;
        struct json_val **array;
        struct json_val_element **object;
    } u;
} json_val_t;

static const char lyric_string_tag[] = {'t', 'a', 'g'};
static const char lyric_string_singers[] = {'s', 'i', 'n', 'g', 'e', 'r', 's'};
static const char lyric_string_content[] = {'c', 'o', 'n', 't', 'e', 'n', 't'};

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
    [JSON_ERROR_CALLBACK] = "error in a callback"
};

static int _json_parser_callback(void *const restrict userdata, const int type, const char *const restrict data, const uint32_t length) {
    struct _Parser *parser = userdata;
    printf("%d %*s\n", type, length, data);
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
                if (length == sizeof(lyric_string_tag) && lyric_strncasecmp(data, lyric_string_tag, sizeof(lyric_string_tag)) == 0) {
                    parser->_d1.t = &parser->_d0->tag;
                    parser->status = lyric_parser_status_lyric_tag;
                } else if (length == sizeof(lyric_string_singers) && lyric_strncasecmp(data, lyric_string_singers, sizeof(lyric_string_singers)) == 0) {
                    parser->status = lyric_parser_status_lyric_singers;
                } else {
                    return JSON_ERROR_CALLBACK;
                }
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_tag: {
            if (type == JSON_OBJECT_BEGIN) {
                parser->status = lyric_parser_status_lyric_tag_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_tag_name: {
            if (type == JSON_KEY) {
                parser->_d2.k = lyric_strndup(data, length);
                parser->status = lyric_parser_status_lyric_tag_value;
            } else if (type == JSON_OBJECT_END) {
                parser->status = lyric_parser_status_lyric;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_tag_value: {
            if (type == JSON_STRING) {
                char *value = lyric_strndup(data, length);
                printf("tag %s=%s\n", parser->_d2.k, value);
                bool result = lyric_tag_insert(parser->_d1.t, parser->_d2.k, value);
                lyric_free(value);
                lyric_free(parser->_d2.k);
                if (!result)
                    return JSON_ERROR_NO_MEMORY;
                parser->status = lyric_parser_status_lyric_tag_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singers: {
            if (type == JSON_ARRAY_BEGIN) {
            } else if (type == JSON_ARRAY_END) {
            } else if (type == JSON_OBJECT_BEGIN) {
                parser->_d1.s = lyric_singer_new();
                if (parser->_d1.s == NULL)
                    return JSON_ERROR_NO_MEMORY;
                parser->status = lyric_parser_status_lyric_singer;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer: {
            if (type == JSON_KEY) {
                if (length == sizeof(lyric_string_tag) && lyric_strncasecmp(data, lyric_string_tag, sizeof(lyric_string_tag)) == 0) {
                    parser->_d2.t = &parser->_d1.s->tag;
                    parser->status = lyric_parser_status_lyric_singer_tag;
                } else if (length == sizeof(lyric_string_content) && lyric_strncasecmp(data, lyric_string_content, sizeof(lyric_string_content)) == 0) {
                    parser->status = lyric_parser_status_lyric_singer_content;
                } else {
                    return JSON_ERROR_CALLBACK;
                }
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_tag: {
            if (type == JSON_OBJECT_BEGIN) {
                parser->status = lyric_parser_status_lyric_singer_tag_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_tag_name: {
            if (type == JSON_KEY) {
                parser->_d3.k = lyric_strndup(data, length);
                parser->status = lyric_parser_status_lyric_singer_tag_value;
            } else if (type == JSON_OBJECT_END) {
                parser->status = lyric_parser_status_lyric_singer;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_tag_value: {
            if (type == JSON_STRING) {
                char *value = lyric_strndup(data, length);
                printf("singer tag %s=%s\n", parser->_d3.k, value);
                bool result = lyric_tag_insert(parser->_d2.t, parser->_d3.k, value);
                lyric_free(value);
                lyric_free(parser->_d3.k);
                if (!result)
                    return JSON_ERROR_NO_MEMORY;
                parser->status = lyric_parser_status_lyric_singer_tag_name;
            } else {
                return JSON_ERROR_CALLBACK;
            }
        } return 0;
        case lyric_parser_status_lyric_singer_content: {
        } return 0;
        case lyric_parser_status_lyric_singer_content_line: {
        } return 0;
        case lyric_parser_status_lyric_singer_content_line_word: {
        } return 0;
        case lyric_parser_status_lyric_singer_content_line_time: {
        } return 0;
        case lyric_parser_status_finish: {
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
    ret = json_parser_is_done(&_parser);
    if (ret != 0) {
        return false;
    }
    return true;
}

Parser *lyric_parser_new(void) {
    Parser *parser = lyric_alloc(sizeof(Parser));
    if (unlikely(parser == NULL))
        return NULL;
    parser->status = lyric_parser_status_finish;
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
    Lyric *result = parser->lyrics;
    lyric_free(parser);
    return result;
}

void lyric_write_file(const Lyric *const restrict lyric, FILE *const restrict file) {
}
