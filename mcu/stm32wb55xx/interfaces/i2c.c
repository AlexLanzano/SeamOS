#include <stdint.h>
#include <libraries/error.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/i2c.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <arch/arch.h>

#define I2C_DEVICE_MAX 8

#define I2C_WRITE 0
#define I2C_READ 1

typedef struct i2c_device {
    bool is_initialized;
    i2c_device_configuration_t device;
} i2c_device_t;

static bool g_i2c_initialized = false;
i2c_device_t g_i2c_devices[I2C_DEVICE_MAX];

static bool i2c_invalid_handle(i2c_handle_t handle)
{
    return (handle >= I2C_DEVICE_MAX || !g_i2c_devices[handle].is_initialized);
}

static error_t i2c_find_free_device(i2c_handle_t *handle)
{
    if (!handle) {
        log_debug("Passed null handle pointer as function argument");
        return ERROR_INVALID;
    }

    i2c_handle_t i2c_handle;

    for (i2c_handle = 0; i2c_handle < I2C_DEVICE_MAX; i2c_handle++) {
        if (!g_i2c_devices[i2c_handle].is_initialized) {
            *handle = i2c_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static bool i2c_data_received(I2C_TypeDef *i2c)
{
    return i2c->ISR & I2C_ISR_RXNE;
}

static bool i2c_ready_to_transmit(I2C_TypeDef *i2c)
{
    return i2c->ISR & I2C_ISR_TXIS;
}

error_t i2c_stop(i2c_handle_t handle)
{
    i2c_device_configuration_t i2c_device = g_i2c_devices[handle].device;
    I2C_TypeDef *i2c = i2c_device.periph_addr;

    arch_disable_irq();
    i2c->CR2 |= I2C_CR2_STOP;
    arch_enable_irq();

    return SUCCESS;
}

error_t i2c_read(i2c_handle_t handle, uint8_t *data, uint32_t size)
{
    if (i2c_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Failed to read from i2c device. Invalid handle");
        return ERROR_INVALID;
    }

    if (!data) {
        log_error(ERROR_INVALID, "Passed null data pointer as function argument");
        return ERROR_INVALID;
    }

    i2c_device_configuration_t i2c_device = g_i2c_devices[handle].device;
    I2C_TypeDef *i2c = i2c_device.periph_addr;
    uint32_t index = 0;

    i2c->CR2 |= i2c_device.address_mode << I2C_CR2_ADD10_Pos;
    if (i2c_device.address_mode == I2C_ADDRESS_MODE_7BIT) {
        i2c->CR2 |= i2c_device.address << (I2C_CR2_SADD_Pos+1);
    } else {
        i2c->CR2 |= i2c_device.address << I2C_CR2_SADD_Pos;
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

error_t i2c_read_byte(i2c_handle_t handle, uint8_t *data)
{
    return i2c_read(handle, data, 1);
}

error_t i2c_write(i2c_handle_t handle, uint8_t *data, uint32_t size)
{
    if (i2c_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Failed to write to i2c device. Invalid handle");
        return ERROR_INVALID;
    }

    if (!data) {
        log_error(ERROR_INVALID, "Passed null data pointer as function argument");
        return ERROR_INVALID;
    }

    i2c_device_configuration_t i2c_device = g_i2c_devices[handle].device;
    I2C_TypeDef *i2c = i2c_device.periph_addr;
    uint32_t index = 0;

    i2c->CR2 |= i2c_device.address_mode << I2C_CR2_ADD10_Pos;
    if (i2c_device.address_mode == I2C_ADDRESS_MODE_7BIT) {
        i2c->CR2 |= i2c_device.address << (I2C_CR2_SADD_Pos+1);
    } else {
        i2c->CR2 |= i2c_device.address << I2C_CR2_SADD_Pos;
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

error_t i2c_write_byte(i2c_handle_t handle, uint8_t data)
{
    return i2c_write(handle, &data, 1);
}

error_t i2c_device_init(i2c_device_configuration_t device, i2c_handle_t *handle)
{
    if (!g_i2c_initialized) {
        log_error(ERROR_INVALID, "I2C bus has not been initialized. Unable to open device");
        return ERROR_INVALID;
    }

    if (!handle) {
        log_error(ERROR_INVALID, "Passed null handle pointer as function argument");
        return ERROR_INVALID;
    }

    error_t error;

    error = i2c_find_free_device(handle);
    if (error) {
        log_error(error, "Failed to open device. No free devices left");
        return error;
    }

    memcpy(&g_i2c_devices[*handle].device, &device, sizeof(i2c_device_configuration_t));
    g_i2c_devices[*handle].is_initialized = true;

    return SUCCESS;
}

error_t i2c_device_deinit(i2c_handle_t handle)
{
    if (i2c_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Unable to close device. Invalid handle");
        return ERROR_INVALID;
    }

    g_i2c_devices[handle].is_initialized = false;

    return SUCCESS;
}

error_t i2c_interface_init(I2C_TypeDef *i2c, uint32_t scl_pin, uint32_t sda_pin, uint32_t timing)
{
    error_t error;
    gpio_configuration_t scl_pin_config = {0};
    gpio_configuration_t sda_pin_config = {0};

    scl_pin_config.mode = GPIO_MODE_ALT_FUNC;
    scl_pin_config.output_type = GPIO_OUTPUT_TYPE_OPEN_DRAIN;
    scl_pin_config.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    scl_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    scl_pin_config.alternative_function = 4;
    error = gpio_init(scl_pin_config, scl_pin);
    if (error) {
        log_error(error, "Failed to configure scl pin");
        return error;
    }

    sda_pin_config.mode = GPIO_MODE_ALT_FUNC;
    sda_pin_config.output_type = GPIO_OUTPUT_TYPE_OPEN_DRAIN;
    sda_pin_config.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    sda_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    sda_pin_config.alternative_function = 4;
    error = gpio_init(sda_pin_config, sda_pin);
    if (error) {
        log_error(error, "Failed to configure sda pin");
        return error;
    }

    i2c->TIMINGR = timing;
    // Enable I2C peripheral
    i2c->CR1 |= I2C_CR1_PE;

    g_i2c_initialized = true;
    return SUCCESS;
}

error_t i2c_deinit()
{
    log_debug("TODO");
    return SUCCESS;
}
