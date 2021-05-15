#ifndef STM32WB55XX_I2C_H
#define STM32WB55XX_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/i2c.h>
#include <stm32wb55xx/gpio.h>
#include <libraries/error.h>

typedef uint32_t i2c_handle_t;

typedef enum i2c_address_mode {
    I2C_ADDRESS_MODE_7BIT,
    I2C_ADDRESS_MODE_10BIT
} i2c_address_mode_t;

typedef struct i2c_interface_configuration {
    I2C_TypeDef *i2c;
    GPIO_TypeDef *scl_port;
    uint8_t scl_pin;

    GPIO_TypeDef *sda_port;
    uint8_t sda_pin;

    uint32_t timing;
} i2c_init_configuration_t;

typedef struct i2c_device_configuration {
    I2C_TypeDef *i2c;
    i2c_address_mode_t address_mode;
    uint16_t address;
} i2c_configuration_t;

#endif
