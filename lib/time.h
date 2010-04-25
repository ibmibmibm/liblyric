#ifndef __TIME_H__
#define __TIME_H__

#include "helpers.h"

struct _Time {
    uint64_t second;
    uint16_t minisecond;
};
typedef struct _Time Time;
Time *lyric_time_new_from_string(const char *string, const size_t length);
bool lyric_time_create_from_string(Time *const restrict time, const char *string, const size_t length);
bool lyric_time_create(Time *const restrict time);
bool lyric_time_copy(Time *const restrict time, const Time *const restrict _time);
void lyric_time_clean(Time *const restrict time);
#endif // __TIME_H__
