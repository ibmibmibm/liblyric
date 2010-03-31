#ifndef __TIME_H__
#define __TIME_H__

#include <stdint.h>
#include <stdio.h>

struct _Time {
    uint8_t minisecond, second, minute, hour;
};
typedef struct _Time Time;

Time* lyric_time_new(void);
Time* lyric_time_new_from_file(FILE *file);
void lyric_time_save_to_file_longform(const Time *time, FILE *file);
void lyric_time_save_to_file_shortform(const Time *time, FILE *file);
void lyric_time_delete(Time *time);

#endif // __TIME_H__
