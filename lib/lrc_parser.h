#ifndef __LRC_PARSER_H__
#define __LRC_PARSER_H__

#include "helpers.h"
#include "lyric.h"
#include "buffer.h"

enum lyric_lrcparser_error {
    lyric_lrcparser_noerror = 0,
    lyric_lrcparser_error_no_memory,
    lyric_lrcparser_error_unexcept_char,
    lyric_lrcparser_error_unknown,
};

enum lyric_lrcparser_status {
    lyric_lrcparser_status_start,
    lyric_lrcparser_status_tag,
    lyric_lrcparser_status_tag_name,
    lyric_lrcparser_status_tag_value,
    lyric_lrcparser_status_time,
    lyric_lrcparser_status_lyric,
    lyric_lrcparser_status_comment,
    lyric_lrcparser_status_finish,
};

struct _LRCParser {
    enum lyric_lrcparser_status status;
    enum lyric_lrcparser_error error;
    size_t lines, col;
    struct _Lyric *lyric;
    struct _Buffer *_buffer;
    struct _Singer *_singer;
    struct _Line *_line;
    char *_n;
};
typedef struct _LRCParser LRCParser;

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
LRCParser *lyric_lrc_parser_new(void);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
LRCParser *lyric_lrc_parser_new_from_file(FILE *const restrict file);

void lyric_lrc_parser_delete(LRCParser *const restrict parser);

LIBLYRIC_WARN_UNUSED_RESULT
bool lyric_lrc_parser_from_file(LRCParser *const restrict parser, FILE *const restrict file);

LIBLYRIC_MALLOC
LIBLYRIC_WARN_UNUSED_RESULT
struct _Lyric *lyric_read_lrc_file(FILE *const restrict file);

void lyric_lrc_parser_clean(LRCParser *const restrict parser);

#endif // __LRC_PARSER_H__
// vim:ts=4 sts=4 sw=4 et
