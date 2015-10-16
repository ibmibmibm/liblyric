#include "lrc_parser.h"

bool lyric_lrc_parser_from_file(LRCParser *const restrict parser, FILE *const restrict file) {
    if (unlikely(parser == NULL || file == NULL)) {
        return false;
    }
    return true;
}

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

// vim:ts=4 sts=4 sw=4 et
