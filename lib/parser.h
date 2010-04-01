#ifndef __PARSER_H__
#define __PARSER_H__

#include "helpers.h"
#include "lyric.h"

enum lyric_parser_error {
    lyric_parser_noerror,
    lyric_parser_error_lyric_start,
    lyric_parser_error_lyric_finish,
    lyric_parser_error_tag_string,
    lyric_parser_error_tag_colon,
    lyric_parser_error_tag_start,
    lyric_parser_error_tag_name,
    lyric_parser_error_tag_name_colon,
    lyric_parser_error_tag_value,
    lyric_parser_error_tag_finish,
    lyric_parser_error_singer_string,
    lyric_parser_error_singer_start,
    lyric_parser_error_singer_name,
    lyric_parser_error_singer_gender,
    lyric_parser_error_singer_content,
    lyric_parser_error_singer_stop,
    lyric_parser_error_string_begin,
    lyric_parser_error_string,
    lyric_parser_error_string_escape,
    lyric_parser_error_string_unicode,
    lyric_parser_error_number,
};

enum lyric_parser_status {
    lyric_parser_status_start,
    lyric_parser_status_whitespace,
    lyric_parser_status_lyric_start,
    lyric_parser_status_lyric_finish,
    lyric_parser_status_tag_string,
    lyric_parser_status_tag_colon,
    lyric_parser_status_tag_start,
    lyric_parser_status_tag_name,
    lyric_parser_status_tag_name_colon,
    lyric_parser_status_tag_value,
    lyric_parser_status_tag_finish,
    lyric_parser_status_singer_string,
    lyric_parser_status_singer_start,
    lyric_parser_status_singer_name,
    lyric_parser_status_singer_gender,
    lyric_parser_status_singer_content,
    lyric_parser_status_singer_stop,
    lyric_parser_status_string_begin,
    lyric_parser_status_string,
    lyric_parser_status_string_escape,
    lyric_parser_status_string_unicode,
    lyric_parser_status_number,
    lyric_parser_status_finish,
};

struct _Parser {
    enum lyric_parser_status status, next_status;
    enum lyric_parser_error error;
    char *buffer;
    char *cursor;
    char *buffer_end;
    size_t length;
    size_t line_number;
    size_t char_number;
    struct _Lyric *lyric;
};
typedef struct _Parser Parser;

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict filename);
Parser *lyric_parser_new_from_file(FILE *const restrict filename);
void lyric_parser_start(Parser *const restrict parser);
struct _Lyric *lyric_read_file(FILE *const restrict file);
void lyric_write_file(const struct _Lyric *const restrict lyric, FILE *const restrict file);

#endif // __PARSER_H__
