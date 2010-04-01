#ifndef __TIME_H__
#define __TIME_H__

#include "helpers.h"

struct _Time {
    uint64_t second;
    uint16_t minisecond;
};
typedef struct _Time Time;

#endif // __TIME_H__
