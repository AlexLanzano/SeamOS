#ifndef DEVICE_H
#define DEVICE_H

#include <stdint.h>
#include <libraries/error.h>

typedef uint32_t device_handle_t;

error_t device_open(const char *name, device_handle_t *handle);
error_t device_close(device_handle_t handle);
error_t device_read(device_handle_t handle, void *data, uint32_t data_length);
error_t device_write(device_handle_t handle, void *data, uint32_t data_length);
error_t device_ioctl(device_handle_t handle, uint32_t cmd, void *arg);

#endif
