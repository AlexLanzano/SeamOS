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

static void log_print(char *format, va_list ap)
{
    if (!format) {
        return;
    }

    char msg_data[LOG_MSG_MAX_LENGTH];
    string_t msg = string_init(msg_data, 0, LOG_MSG_MAX_LENGTH);
    string_t format_string = string(format);

    if (msg.error || format_string.error) {
        return;
    }

    string_format(&msg, format_string, ap);

    lpuart_write(g_lpuart_handle, (uint8_t *)string_data(msg), string_size(msg));
}

void log(log_level_t log_level, char *format, ...)
{
    if (g_log_level < log_level || !format) {
        return;
    }
    va_list ap;
    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
}

void log_error(error_t error, char *format, ...)
{
    if (g_log_level < LOG_LEVEL_ERROR || !format) {
        return;
    }

    va_list ap;

    log(LOG_LEVEL_ERROR, "ERROR %s [%s]: ", string((char *)__func__), string((char *)error_get_message(error)));
    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
    log(LOG_LEVEL_ERROR, "\r\n");
}

void log_info(char *format, ...)
{
    if (g_log_level < LOG_LEVEL_INFO || !format) {
        return;
    }

    va_list ap;

    log(LOG_LEVEL_INFO, "INFO %s: ", __func__);
    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
    log(LOG_LEVEL_INFO, "\r\n");
}

void log_debug(char *format, ...)
{
    if (g_log_level < LOG_LEVEL_DEBUG || !format) {
        return;
    }

    va_list ap;

    log(LOG_LEVEL_DEBUG, "DEBUG %s: ", __func__);
    va_start(ap, format);
    log_print(format, ap);
    va_end(ap);
    log(LOG_LEVEL_DEBUG, "\r\n");
}

uint8_t log_wait_for_input()
{
    uint8_t data;
    while (lpuart_rx_empty());
    lpuart_read(g_lpuart_handle, &data);
    return data;
}

void log_init(log_configuration_t config)
{
    g_lpuart_handle = config.lpuart_handle;
}