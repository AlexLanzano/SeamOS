#ifndef STM32WB55XX_I2C_H
#define STM32WB55XX_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/interfaces/i2c.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <libraries/error.h>

typedef uint32_t i2c_handle_t;

typedef enum i2c_address_mode {
    I2C_ADDRESS_MODE_7BIT,
    I2C_ADDRESS_MODE_10BIT
} i2c_address_mode_t;

typedef struct i2c_device_configuration {
    I2C_TypeDef *periph_addr;
    i2c_address_mode_t address_mode;
    uint16_t address;
} i2c_configuration_t;

error_t i2c_interface_init(I2C_TypeDef *i2c, uint32_t scl_pin, uint32_t sda_pin, uint32_t timing);
#endif
