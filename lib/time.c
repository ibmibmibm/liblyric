#include <stdlib.h>
#include "time.h"

Time* lyric_time_new(void) {
    Time *time = (Time*)calloc(sizeof(Time), 1);
    if (time == NULL)
        return NULL;
    return time;
}

Time* lyric_time_new_from_file(FILE *file) {
    Time *time = (Time*)calloc(sizeof(Time), 1);
    if (time == NULL)
        return NULL;
    return time;
}

void lyric_time_save_to_file_longform(const Time *time, FILE *file) {
    if (time == NULL)
        return;
    fprintf(file, "%d:%02d:%02d.%02d", (int)time->hour, (int)time->minute, (int)time->second, (int)time->minisecond);
}

void lyric_time_save_to_file_shortform(const Time *time, FILE *file) {
    int second;
    if (time == NULL)
        return;
    second = time->hour;
    second = second * 60 + time->minute;
    second = second * 60 + time->second;
    fprintf(file, "%d.%02d", second, (int)time->minisecond);
}

void lyric_time_delete(Time *time) {
    if (time == NULL)
        return;
    free(time);
}
