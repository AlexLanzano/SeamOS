#include <stdint.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <mcu/interfaces/i2c.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/interfaces/i2c.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <arch/arch.h>

#define MAX_I2C_INTERFACES 2

I2C_TypeDef *g_i2c_interfaces[MAX_I2C_INTERFACES] = {
    I2C1,
    I2C3 // Yeah.. there is no I2C2
};

static bool i2c_invalid_handle(uint32_t handle)
{
    return handle >= MAX_I2C_INTERFACES;
}

static bool i2c_data_received(I2C_TypeDef *i2c)
{
    return i2c->ISR & I2C_ISR_RXNE;
}

static bool i2c_ready_to_transmit(I2C_TypeDef *i2c)
{
    return i2c->ISR & I2C_ISR_TXIS;
}

error_t i2c_stop(uint32_t handle)
{
    if (i2c_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    I2C_TypeDef *i2c = g_i2c_interfaces[handle];

    arch_disable_irq();
    i2c->CR2 |= I2C_CR2_STOP;
    arch_enable_irq();

    return SUCCESS;
}

error_t i2c_read(uint32_t handle, i2c_configuration_t *config, uint8_t *data, uint32_t size)
{
    if (i2c_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    if (!data) {
        return ERROR_INVALID;
    }

    I2C_TypeDef *i2c = g_i2c_interfaces[handle];
    uint32_t index = 0;

    i2c->CR2 |= config->address_mode << I2C_CR2_ADD10_Pos;
    if (config->address_mode == I2C_ADDRESS_MODE_7BIT) {
        i2c->CR2 |= config->address << (I2C_CR2_SADD_Pos+1);
    } else {
        i2c->CR2 |= config->address << I2C_CR2_SADD_Pos;
    }
    i2c->CR2 |= I2C_CR2_RD_WRN;
    // TODO: Add capability to have over 256 bytes
    i2c->CR2 |= (size & 0xFF) << I2C_CR2_NBYTES_Pos;
    i2c->CR2 |= I2C_CR2_START;
    // TODO: we definitely need a timeout here
    while (size) {
        if (!i2c_data_received(i2c)) {
            continue;
        }

        data[index++] = i2c->RXDR;
        size--;
    }
    return SUCCESS;
}

error_t i2c_read_byte(uint32_t handle, i2c_configuration_t *config, uint8_t *data)
{
    if (i2c_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    return i2c_read(handle, config, data, 1);
}

error_t i2c_write(uint32_t handle, i2c_configuration_t *config, uint8_t *data, uint32_t size)
{
    if (i2c_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    if (!data) {
        return ERROR_INVALID;
    }

    I2C_TypeDef *i2c = g_i2c_interfaces[handle];
    uint32_t index = 0;

    i2c->CR2 |= config->address_mode << I2C_CR2_ADD10_Pos;
    if (config->address_mode == I2C_ADDRESS_MODE_7BIT) {
        i2c->CR2 |= config->address << (I2C_CR2_SADD_Pos+1);
    } else {
        i2c->CR2 |= config->address << I2C_CR2_SADD_Pos;
    }
    i2c->CR2 &= ~I2C_CR2_RD_WRN;
    // TODO: Add capability to have over 256 bytes
    i2c->CR2 &= ~I2C_CR2_NBYTES;
    i2c->CR2 |= size << I2C_CR2_NBYTES_Pos;
    i2c->CR2 |= I2C_CR2_START;

    // TODO: we definitely need a timeout here
    while (index < size) {
        if (!i2c_ready_to_transmit(i2c)) {
            continue;
        }

        i2c->TXDR = data[index++];
    }
    while ((i2c->ISR & I2C_ISR_TC) == 0);
    return SUCCESS;
}

error_t i2c_write_byte(uint32_t handle, i2c_configuration_t *config, uint8_t data)
{
    return i2c_write(handle, config, &data, 1);
}

error_t i2c_enable(I2C_TypeDef *i2c, uint32_t scl_pin, uint32_t sda_pin, uint32_t timing)
{
    error_t error;
    gpio_configuration_t scl_pin_config = {0};
    gpio_configuration_t sda_pin_config = {0};

    scl_pin_config.mode = GPIO_MODE_ALT_FUNC;
    scl_pin_config.output_type = GPIO_OUTPUT_TYPE_OPEN_DRAIN;
    scl_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    scl_pin_config.alternate_function = GPIO_ALT_FUNC_4_I2C;
    error = gpio_init(scl_pin, &scl_pin_config);
    if (error) {
        return error;
    }

    sda_pin_config.mode = GPIO_MODE_ALT_FUNC;
    sda_pin_config.output_type = GPIO_OUTPUT_TYPE_OPEN_DRAIN;
    sda_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    sda_pin_config.alternate_function = GPIO_ALT_FUNC_4_I2C;
    error = gpio_init(sda_pin, &sda_pin_config);
    if (error) {
        return error;
    }

    // TODO: LOOK INTO THIS IF THINGS DONT WORK
    i2c->TIMINGR = timing;
    // Enable I2C peripheral
    i2c->CR1 |= I2C_CR1_PE;

    return SUCCESS;
}

error_t i2c_disable()
{
    return SUCCESS;
}
