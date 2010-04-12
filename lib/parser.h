#ifndef __PARSER_H__
#define __PARSER_H__

#include "helpers.h"
#include "lyric.h"

enum lyric_parser_error {
    lyric_parser_noerror = 0,
    lyric_parser_error_no_memory,
};

enum lyric_parser_status {
    lyric_parser_status_start,
    lyric_parser_status_lyric,
    lyric_parser_status_lyric_tag,
    lyric_parser_status_lyric_tag_entry,
    lyric_parser_status_lyric_singers,
    lyric_parser_status_lyric_singer,
    lyric_parser_status_lyric_singer_tag,
    lyric_parser_status_lyric_singer_tag_entry,
    lyric_parser_status_lyric_singer_content,
    lyric_parser_status_lyric_singer_content_line,
    lyric_parser_status_lyric_singer_content_line_word,
    lyric_parser_status_lyric_singer_content_line_time,
    lyric_parser_status_finish,
};

struct _Parser {
    enum lyric_parser_status status;
    enum lyric_parser_error error;
    void *current;
    size_t lines, col;
    struct _Lyric *lyric;
};
typedef struct _Parser Parser;

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict filename);
Parser *lyric_parser_new_from_file(FILE *const restrict filename);
struct _Lyric *lyric_read_file(FILE *const restrict file);
void lyric_write_file(const struct _Lyric *const restrict lyric, FILE *const restrict file);

#endif // __PARSER_H__
