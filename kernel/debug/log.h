#ifndef LOG_H
#define LOG_H

#include <stdarg.h>
#include <stdint.h>
#include <mcu/lpuart.h>
#include <libraries/error.h>

#define LOG_MSG_MAX_LENGTH 512

typedef enum {
    LOG_LEVEL_ERROR,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
} log_level_t;

typedef struct log_configuration {
    lpuart_handle_t lpuart_handle;
} log_configuration_t;

void log(log_level_t log_level, char *format, ...);
void log_error(error_t error, char *format, ...);
void log_info(char *format, ...);
void log_debug(char *format, ...);
void log_init(log_configuration_t config);

#endif
