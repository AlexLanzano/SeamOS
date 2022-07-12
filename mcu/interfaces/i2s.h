#ifndef I2S_H
#define I2S_H

#include <stdint.h>
#include <libraries/error.h>

typedef struct i2s_interface_configuration i2s_interface_configration_t;
typedef struct i2s_device_configuration i2s_device_configuration_t;

typedef uint32_t i2s_interface_handle_t;
typedef uint32_t i2s_device_handle_t;

error_t i2s_interface_init(i2s_interface_configuration_t config, i2s_interface_handle_t *handle);
error_t i2s_interface_deinit(i2s_interface_handle_t handle);

error_t i2s_device_init(i2s_device_configuration_t config, i2s_device_handle_t *handle);
error_t i2s_device_deinit(i2s_device_handle_t handle);
error_t i2s_device_read(i2s_device_handle_t handle, void *buffer, uint32_t length);
error_t i2s_device_write(i2s_device_handle_t handle, void *data, uint32_t length);

#endif
