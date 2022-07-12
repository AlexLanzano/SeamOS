#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/device/device.h>
#include <mcu/interfaces/uart.h>

static error_t log_init(uint32_t interface_handle)
{
    return uart_init(interface_handle, 0);
    
}

static error_t log_deinit(uint32_t interface_handle)
{
    return uart_deinit(interface_handle);
}

static error_t log_read(uint32_t interface_handle, uint8_t *data, uint32_t data_length)
{
    return uart_read(interface_handle, data, data_length);
}

static error_t log_write(uint32_t interface_handle, uint8_t *data, uint32_t data_length)
{
    return uart_write(interface_handle, data, data_length);
}

static error_t log_ioctl(uint32_t interface_handle, uint32_t cmd, void *arg)
{
    return ERROR_NOT_IMPLEMENTED;
}

device_ops_t g_log_device_ops = (device_ops_t){.init = log_init,
                                               .deinit = log_deinit,
                                               .read = log_read,
                                               .write = log_write,
                                               .ioctl = log_ioctl};
