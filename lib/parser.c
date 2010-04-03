#include "parser.h"
#include "json.h"

static inline void lyric_parser_switch(Parser *const restrict parser);

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict file) {
    if (unlikely(parser == NULL || file == NULL))
        return false;
    parser->root = json_object_from_file(file);
    if ((ptrdiff_t)parser->root < 0)
        return false;
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

static void lyric_parser_lyric_tag(Parser *const restrict parser) {
}

static void lyric_parser_lyric_singers(Parser *const restrict parser) {
}

static void lyric_parser_lyric(Parser *const restrict parser) {
    if (json_object_is_type(parser->current, json_type_object)) {
        parser->current = json_object_object_get(parser->current, "tag");
        if (parser->current != NULL) {
            lyric_parser_lyric_tag(parser);
        }
        parser->current = json_object_object_get(parser->current, "singers");
        if (parser->current != NULL) {
            lyric_parser_lyric_singers(parser);
        }
    }
}

void lyric_parser_start(Parser *const restrict parser) {
    if (unlikely(parser == NULL)) {
        return;
    }
    if (json_object_is_type(parser->root, json_type_array)) {
        const size_t length = json_object_array_length(parser->root);
        for (size_t i = 0; i < length; ++i) {
            parser->current = json_object_array_get_idx(parser->root, i);
            lyric_parser_lyric(parser);
        }
    } else {
        lyric_parser_lyric(parser);
    }
}

Lyric *lyric_read_file(FILE *const restrict file) {
    Parser *parser = lyric_parser_new_from_file(file);
    if (unlikely(parser == NULL))
        return NULL;
    lyric_parser_start(parser);
    if (parser->status == lyric_parser_status_finish) {
        Lyric *result = parser->lyric;
        json_object_put(parser->root);
        lyric_free(parser);
        return result;
    } else {
        lyric_free(parser->lyric);
        json_object_put(parser->root);
        lyric_free(parser);
        return NULL;
    }
}

void lyric_write_file(const Lyric *const restrict lyric, FILE *const restrict file) {
}
