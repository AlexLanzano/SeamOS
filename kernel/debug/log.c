#include <stdarg.h>
#include <config.h>
#include <mcu/lpuart.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <libraries/error.h>

#ifdef CONFIG_LOG_LEVEL
log_level_t g_log_level = CONFIG_LOG_LEVEL;
#else
log_level_t g_log_level = LOG_LEVEL_INFO;
#endif

int32_t g_lpuart_handle = -1;

static void log_print(const char *format, va_list ap)
{
    if (!format) {
        return;
    }

    char msg_data[LOG_MSG_MAX_LENGTH] = {0};
    char format_data[LOG_MSG_MAX_LENGTH] = {0};
    string_t msg = string_init(msg_data, LOG_MSG_MAX_LENGTH);
    string_t format_string = string_init_from_cstring(format_data, format, LOG_MSG_MAX_LENGTH);

    if (msg.error || format_string.error) {
        return;
    }

    string_format(&msg, format_string, ap);
    string_concatenate_cstring(&msg, "\r\n", 2);

    lpuart_write(g_lpuart_handle, (uint8_t *)msg.data, msg.size);
}

void log(log_level_t log_level, const char *format, ...)
{
    if (g_log_level < log_level || !format || format[0] == 0) {
        return;
    }
    va_list ap;
    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
}

void log_error(error_t error, const char *format, ...)
{
    if (g_log_level < LOG_LEVEL_ERROR || !format || format[0] == 0) {
        return;
    }

    va_list ap;

    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
}

void log_info(const char *format, ...)
{
    if (g_log_level < LOG_LEVEL_INFO || !format || format[0] == 0) {
        return;
    }

    va_list ap;

    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
}

void log_debug(const char *format, ...)
{
    if (g_log_level < LOG_LEVEL_DEBUG || !format || format[0] == 0) {
        return;
    }

    va_list ap;

    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
}

void log_init(log_configuration_t config)
{
    g_lpuart_handle = config.lpuart_handle;
}
