#ifndef TIME_H
#define TIME_H

#include <stdint.h>

typedef struct datetime {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t weekday;
} datetime_t;

#endif
