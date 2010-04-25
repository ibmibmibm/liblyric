#include "time.h"

Time *lyric_time_new_from_string(const char *string, const size_t length) {
    Time *time = (Time*)lyric_alloc(sizeof(Time));
    if (unlikely(time == NULL)) {
        goto err0;
    }
    if (unlikely(!lyric_time_create_from_string(time, string, length))) {
        goto err1;
    }
    return time;
err1:
    lyric_free(time);
err0:
    return NULL;
}

bool lyric_time_create_from_string(Time *const restrict time, const char *const restrict string, const size_t length) {
    if (unlikely(time == NULL)) {
        return false;
    }
    time->second = time->minisecond = 0;
    size_t status = 0;
    size_t counter = 0;
    for (size_t i = 0; i < length; ++i) {
        printf("i:%2zd,string[i]: %c,status:%2zd\n", i, string[i], status);
        switch (status) {
            case 0: {
                if (isdigit(string[i])) {
                    time->second = time->second * 10 + (string[i] - '0');
                } else if (string[i] == ':') {
                    time->second *= 60;
                    status = 1;
                } else {
                    return false;
                }
            } break;
            case 1: {
                if (isdigit(string[i])) {
                    if (counter == 2) {
                        return false;
                    }
                    time->second = time->second * 10 + (string[i] - '0');
                } else if (string[i] == ':') {
                    if (counter != 2) {
                        return false;
                    }
                    time->second *= 60;
                    status = 2;
                    counter = 0;
                } else {
                    return false;
                }
                ++counter;
            } break;
            case 2: {
                if (isdigit(string[i])) {
                    if (counter == 2) {
                        return false;
                    }
                    time->second = time->second * 10 + (string[i] - '0');
                } else if (string[i] == '.') {
                    if (counter != 2) {
                        return false;
                    }
                    status = 3;
                    counter = 0;
                } else {
                    return false;
                }
                ++counter;
            } break;
            case 3: {
                if (isdigit(string[i])) {
                    if (counter == 3) {
                        return false;
                    }
                    time->minisecond = time->minisecond * 10 + (string[i] - '0');
                } else {
                    return false;
                }
                ++counter;
            } break;
        }
    }
    return true;
}

bool lyric_time_create(Time *const restrict time) {
    time->second = time->minisecond = 0;
    return true;
}

bool lyric_time_copy(Time *const restrict time, const Time *const restrict _time) {
    if (unlikely(time == NULL || _time == NULL)) {
        goto err0;
    }
    if (unlikely(time == _time)) {
        return true;
    }
    time->second = _time->second;
    time->minisecond = _time->minisecond;
    return true;
err0:
    return false;
}

void lyric_time_clean(Time* const restrict time) {
}
