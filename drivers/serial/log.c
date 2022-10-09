#include <drivers/serial/log.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/device/device.h>
#include <mcu/interfaces/uart.h>

static error_t log_init(void *config)
{
    if (!config) {
        return ERROR_INVALID;
    }
    log_configuration_t *log_config = (log_configuration_t *)config;
    return uart_init(log_config->interface_handle, 0);
}

static error_t log_deinit(void *config)
{
    if (!config) {
        return ERROR_INVALID;
    }
    log_configuration_t *log_config = (log_configuration_t *)config;
    return uart_deinit(log_config->interface_handle);
}

static error_t log_read(void *config, void *buffer, uint32_t data_length)
{
    if (!config) {
        return ERROR_INVALID;
    }
    log_configuration_t *log_config = (log_configuration_t *)config;
    uint8_t *data = (uint8_t *)buffer;
    return uart_read(log_config->interface_handle, data, data_length);
}

static error_t log_write(void *config, void *buffer, uint32_t data_length)
{
    if (!config) {
        return ERROR_INVALID;
    }

    log_configuration_t *log_config = (log_configuration_t *)config;
    uint8_t *data = (uint8_t *)buffer;
    return uart_write(log_config->interface_handle, data, data_length);
}

static error_t log_ioctl(void *config, uint32_t cmd, void *arg)
{
    return ERROR_NOT_IMPLEMENTED;
}

device_ops_t g_log_device_ops = (device_ops_t){.init = log_init,
                                               .deinit = log_deinit,
                                               .read = log_read,
                                               .write = log_write,
                                               .ioctl = log_ioctl};
