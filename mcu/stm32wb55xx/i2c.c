#include <stdint.h>
#include <libraries/error.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <stm32wb55xx.h>
#include <stm32wb55xx/i2c.h>
#include <stm32wb55xx/gpio.h>
#include <stm32wb55xx/rcc.h>
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
    I2C_TypeDef *i2c = i2c_device.i2c;

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
    I2C_TypeDef *i2c = i2c_device.i2c;
    uint32_t index = 0;

    arch_disable_irq();
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
    arch_enable_irq();
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
    I2C_TypeDef *i2c = i2c_device.i2c;
    uint32_t index = 0;

    arch_disable_irq();
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
    arch_enable_irq();
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

error_t i2c_interface_init(i2c_interface_configuration_t config)
{
    error_t error;
    gpio_handle_t handle;
    gpio_configuration_t scl_pin;
    gpio_configuration_t sda_pin;
    I2C_TypeDef *i2c = config.i2c;
    memset(&scl_pin, 0, sizeof(gpio_configuration_t));
    memset(&sda_pin, 0, sizeof(gpio_configuration_t));

    scl_pin.port = config.scl_port;
    scl_pin.pin = config.scl_pin;
    scl_pin.mode = GPIO_MODE_ALT_FUNC;
    scl_pin.output_type = GPIO_OUTPUT_TYPE_OPEN_DRAIN;
    scl_pin.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    scl_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    scl_pin.alternative_function = 4;
    error = gpio_configure_pin(scl_pin, &handle);
    if (error) {
        log_error(error, "Failed to configure scl pin");
        return error;
    }

    sda_pin.port = config.sda_port;
    sda_pin.pin = config.sda_pin;
    sda_pin.mode = GPIO_MODE_ALT_FUNC;
    sda_pin.output_type = GPIO_OUTPUT_TYPE_OPEN_DRAIN;
    sda_pin.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    sda_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    sda_pin.alternative_function = 4;
    error = gpio_configure_pin(sda_pin, &handle);
    if (error) {
        log_error(error, "Failed to configure sda pin");
        return error;
    }

    i2c->TIMINGR = config.timing;
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
