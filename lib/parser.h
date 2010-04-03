#ifndef __PARSER_H__
#define __PARSER_H__

#include "helpers.h"
#include "lyric.h"

enum lyric_parser_error {
    lyric_parser_noerror,
};

enum lyric_parser_status {
    lyric_parser_status_start,
    lyric_parser_status_finish,
};

struct _Parser {
    enum lyric_parser_status status, next_status;
    enum lyric_parser_error error;
    struct json_object *root;
    struct json_object *current;
    struct _Lyric *lyric;
};
typedef struct _Parser Parser;

bool lyric_parser_from_file(Parser *const restrict parser, FILE *const restrict filename);
Parser *lyric_parser_new_from_file(FILE *const restrict filename);
void lyric_parser_start(Parser *const restrict parser);
struct _Lyric *lyric_read_file(FILE *const restrict file);
void lyric_write_file(const struct _Lyric *const restrict lyric, FILE *const restrict file);

#endif // __PARSER_H__
