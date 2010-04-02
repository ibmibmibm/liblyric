#include "parser.h"
#include "json.h"

static inline void lyric_parser_switch(Parser *const restrict parser);

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict file) {
    if (unlikely(parser == NULL || file == NULL))
        return false;
    parser->json = json_object_from_file(file);
    if ((ptrdiff_t)parser->json < 0)
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

static void lyric_parser_switch(Parser *const restrict parser) {
    switch (parser->status) {
    }
}

void lyric_parser_start(Parser *const restrict parser) {
    if (unlikely(parser == NULL)) {
        return;
    }
    parser->status = lyric_parser_status_finish;
    return lyric_parser_switch(parser);
}

Lyric *lyric_read_file(FILE *const restrict file) {
    Parser *parser = lyric_parser_new_from_file(file);
    if (unlikely(parser == NULL))
        return NULL;
    lyric_parser_start(parser);
    if (parser->status == lyric_parser_status_finish) {
        Lyric *result = parser->lyric;
        json_object_put(parser->json);
        lyric_free(parser);
        return result;
    } else {
        lyric_free(parser->lyric);
        json_object_put(parser->json);
        lyric_free(parser);
        return NULL;
    }
}

void lyric_write_file(const Lyric *const restrict lyric, FILE *const restrict file) {
}
