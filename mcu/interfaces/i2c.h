#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <libraries/error.h>

typedef enum i2c_address_mode {
    I2C_ADDRESS_MODE_7BIT,
    I2C_ADDRESS_MODE_8BIT,
    I2C_ADDRESS_MODE_10BIT
} i2c_address_mode_t;

typedef struct i2c_configuration {
    uint32_t address;
    i2c_address_mode_t address_mode;
} i2c_configuration_t;

error_t i2c_stop(uint32_t handle);
error_t i2c_read(uint32_t handle, i2c_configuration_t *config, uint8_t *data, uint32_t size);
error_t i2c_read_byte(uint32_t handle, i2c_configuration_t *config, uint8_t *data);
error_t i2c_write(uint32_t handle, i2c_configuration_t *config, uint8_t *data, uint32_t size);
error_t i2c_write_byte(uint32_t handle, i2c_configuration_t *config, uint8_t data);

#endif
