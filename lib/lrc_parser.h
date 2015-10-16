#ifndef __LRC_PARSER_H__
#define __LRC_PARSER_H__

#include "helpers.h"
#include "lyric.h"

struct _LRCParser {
    struct _Lyric *lyric;
};
typedef struct _LRCParser LRCParser;

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
LRCParser *lyric_lrc_parser_new(void);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
LRCParser *lyric_lrc_parser_new_from_file(FILE *const restrict file);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_lrc_parser_from_file(LRCParser *const restrict parser, FILE *const restrict file);

void lyric_lrc_parser_delete(LRCParser *const restrict parser);

#endif // __LRC_PARSER_H__
// vim:ts=4 sts=4 sw=4 et
