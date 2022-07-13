#ifndef DEVICE_H
#define DEVICE_H

#include <config.h>
#include <stdbool.h>
#include <stdint.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/device/device.h>

typedef uint32_t device_handle_t;

typedef struct device_ops {
    error_t (*init)(void *config);
    error_t (*deinit)(void *config);
    error_t (*read)(void *config, uint8_t *data, uint32_t data_length);
    error_t (*write)(void *config, uint8_t *data, uint32_t data_length);
    error_t (*ioctl)(void *config, uint32_t cmd, void *arg);
} device_ops_t;

typedef struct device {
    bool is_initialized;
    string_t name;
    char name_data[CONFIG_DEVICE_NAME_LENGTH];
    device_ops_t *ops;
    void *config;
} device_t;

error_t device_init(const char *name, device_ops_t *ops, void *config);
error_t device_deinit(const char *name);
error_t device_open(const char *name, device_handle_t *handle);
error_t device_close(device_handle_t handle);
error_t device_read(device_handle_t handle, uint8_t *data, uint32_t data_length);
error_t device_write(device_handle_t handle, uint8_t *data, uint32_t data_length);
error_t device_ioctl(device_handle_t handle, uint32_t cmd, void *arg);

#endif
