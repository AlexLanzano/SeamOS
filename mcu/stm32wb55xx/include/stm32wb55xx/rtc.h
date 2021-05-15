#ifndef RTC_H
#define RTC_H

#include <stdint.h>
#include <libraries/time.h>
#include <libraries/error.h>

error_t rtc_set_date_and_time(datetime_t *datetime);
error_t rtc_get_date_and_time(datetime_t *datetime);
error_t rtc_init();

#endif
