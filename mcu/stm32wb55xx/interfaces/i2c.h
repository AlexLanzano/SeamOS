#ifndef STM32WB55XX_I2C_H
#define STM32WB55XX_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include <libraries/error.h>

error_t i2c_enable(I2C_TypeDef *i2c, uint32_t scl_pin, uint32_t sda_pin, uint32_t timing);
error_t i2c_disable();

#endif
