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
    lyric_parser_status_lyric_tags,
    lyric_parser_status_lyric_tags_name,
    lyric_parser_status_lyric_tags_value,
    lyric_parser_status_lyric_singers,
    lyric_parser_status_lyric_singer,
    lyric_parser_status_lyric_singer_tags,
    lyric_parser_status_lyric_singer_tags_name,
    lyric_parser_status_lyric_singer_tags_value,
    lyric_parser_status_lyric_singer_contents,
    lyric_parser_status_lyric_singer_contents_line,
    lyric_parser_status_lyric_singer_contents_line_offset,
    lyric_parser_status_lyric_singer_contents_line_word,
    lyric_parser_status_lyric_singer_contents_line_time,
    lyric_parser_status_finish,
};

struct _Parser {
    enum lyric_parser_status status;
    enum lyric_parser_error error;
    size_t lines, col;
    struct _Lyric *lyrics;
    size_t size;
    size_t _malloc_size;
    struct _Lyric *_d0;
    union {
        struct _Tags *t;
        struct _Singer *s;
    } _d1;
    union {
        char *k;
        struct _Tags *t;
        struct _Line *l;
    } _d2;
    union {
        char *k;
    } _d3;
};
typedef struct _Parser Parser;

Parser *lyric_parser_new(void);
bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict filename);
Parser *lyric_parser_new_from_file(FILE *const restrict filename);
struct _Lyric *lyric_read_file(FILE *const restrict file);
void lyric_write_file(const struct _Lyric *const restrict lyric, FILE *const restrict file);
void lyric_parser_clean(Parser *const restrict parser);
void lyric_parser_delete(Parser *const restrict parser);

#endif // __PARSER_H__
