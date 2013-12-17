#ifndef __TIME_H__
#define __TIME_H__

#include "helpers.h"

struct _Time {
    uint64_t second;
    uint16_t minisecond;
};
typedef struct _Time Time;

bool lyric_time_create_from_string(Time *const restrict time, const char *string, const size_t length);
bool lyric_time_create_from_literal(Time *const restrict time, const char *string, const size_t length);
bool lyric_time_create(Time *const restrict time);
bool lyric_time_copy(Time *const restrict time, const Time *const restrict _time);

size_t lyric_time_to_literal(const Time *const restrict time);
char *lyric_time_to_new_string(const Time *const restrict time);
#endif // __TIME_H__
// vim:ts=4 sts=4 sw=4 et
