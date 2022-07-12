#include <stdint.h>
#include <libraries/device.h>
#include <libraries/error.h>
#include <arch/system.h>

error_t device_open(const char *name, device_handle_t *handle)
{
    if (!name || !handle) {
        return ERROR_INVALID;
    }

    device_open_args_t args = (device_open_args_t){.name = name, .handle = handle};
    system_call(SYSTEM_CALL_DEVICE_OPEN, &args);
    return SUCCESS;
}

error_t device_close(device_handle_t handle)
{
    device_close_args_t args = (device_close_args_t){.handle = handle};
    system_call(SYSTEM_CALL_DEVICE_CLOSE, &args);
    return SUCCESS;
}

error_t device_read(device_handle_t handle, uint8_t *data, uint32_t data_length)
{
    if (!data) {
        return ERROR_INVALID;
    }

    device_read_args_t args = (device_read_args_t){.handle = handle, .data = data, .data_length = data_length};
    system_call(SYSTEM_CALL_DEVICE_READ, &args);
    return SUCCESS;
}

error_t device_write(device_handle_t handle, uint8_t *data, uint32_t data_length)
{
    if (!data) {
        return ERROR_INVALID;
    }

    device_write_args_t args = (device_write_args_t){.handle = handle, .data = data, .data_length = data_length};
    system_call(SYSTEM_CALL_DEVICE_WRITE, &args);
    return SUCCESS;
}

error_t device_ioctl(device_handle_t handle, uint32_t cmd, void *arg)
{
    if (!arg) {
        return ERROR_INVALID;
    }

    device_ioctl_args_t args = (device_ioctl_args_t){.handle = handle, .cmd = cmd, .arg = arg};
    system_call(SYSTEM_CALL_DEVICE_IOCTL, &args);
    return SUCCESS;
}
