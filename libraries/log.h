#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdint.h>
#include <mcu/interfaces/uart.h>
#include <libraries/error.h>

#define LOG_MSG_MAX_LENGTH 512

typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} log_level_t;

error_t log_init();
void log(log_level_t log_level, const char *format, ...);
void log_error(error_t error, const char *format, ...);
void log_info(const char *format, ...);
void log_debug(const char *format, ...);

#endif
