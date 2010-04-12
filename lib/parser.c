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

static int _json_parser_callback(void *userdata, int type, const char *data, uint32_t length) {
    printf("%d %*s\n", type, length, data);
    return 0;
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
            if (buffer[i] == '\n') { *col = 0; *lines++; } else *col++;
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
    int ret = json_parser_init(&_parser, &config, _json_parser_callback, NULL);
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

Lyric *lyric_read_file(FILE *const restrict file) {
    Parser *parser = lyric_parser_new_from_file(file);
    if (unlikely(parser == NULL))
        return NULL;
    Lyric *result = parser->lyric;
    lyric_free(parser);
    return result;
}

void lyric_write_file(const Lyric *const restrict lyric, FILE *const restrict file) {
}
