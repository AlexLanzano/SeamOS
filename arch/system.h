#ifndef SYSTEM_H
#define SYSTEM_H

#include <kernel/device/device.h>

typedef enum system_call {
    SYSTEM_CALL_DEVICE_OPEN,
    SYSTEM_CALL_DEVICE_CLOSE,
    SYSTEM_CALL_DEVICE_READ,
    SYSTEM_CALL_DEVICE_WRITE,
    SYSTEM_CALL_DEVICE_IOCTL,
} system_call_t;

typedef struct device_open_args {
    const char *name;
    device_handle_t *handle;
} device_open_args_t;


typedef struct device_close_args {
    device_handle_t handle;
} device_close_args_t;


typedef struct device_read_args {
    device_handle_t handle;
    uint8_t *data;
    uint32_t data_length;
} device_read_args_t;

typedef struct device_write_args {
    device_handle_t handle;
    uint8_t *data;
    uint32_t data_length;
} device_write_args_t;

typedef struct device_ioctl_args {
    device_handle_t handle;
    uint32_t cmd;
    void *arg;
} device_ioctl_args_t;

void system_call(system_call_t system_call, void *args);

#endif
