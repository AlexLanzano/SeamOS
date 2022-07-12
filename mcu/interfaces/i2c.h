#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <libraries/error.h>

typedef uint32_t i2c_handle_t;
typedef struct i2c_interface_configuration i2c_interface_configuration_t;
typedef struct i2c_device_configuration i2c_device_configuration_t;

error_t i2c_stop(i2c_handle_t handle);
error_t i2c_read(i2c_handle_t handle, uint8_t *data, uint32_t size);
error_t i2c_read_byte(i2c_handle_t handle, uint8_t *data);
error_t i2c_write(i2c_handle_t handle, uint8_t *data, uint32_t size);
error_t i2c_write_byte(i2c_handle_t handle, uint8_t data);
error_t i2c_device_init(i2c_device_configuration_t device, i2c_handle_t *handle);
error_t i2c_device_deinit(i2c_handle_t handle);

#endif
