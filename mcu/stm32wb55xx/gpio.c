#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef CONFIG_GPIO_PIN_COUNT
#define CONFIG_GPIO_PIN_COUNT 16
#endif

gpio_configuration_t g_gpio_pins[CONFIG_GPIO_PIN_COUNT];

static error_t gpio_set_mode(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_mode_t mode)
{
    if (pin > 15 || !gpio_port) {
        return ERROR_INVALID;
    }

    gpio_port->MODER &= ~(3 << (pin<<1));
    gpio_port->MODER |= mode << (pin<<1);

    return SUCCESS;
}

static error_t gpio_set_output_type(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_output_type_t output_type)
{
    if (pin > 15 || !gpio_port) {
        return ERROR_INVALID;
    }

    gpio_port->OTYPER &= ~(1 << pin);
    gpio_port->OTYPER |= output_type << pin;

    return SUCCESS;
}

static error_t gpio_set_output_speed(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_output_speed_t output_speed)
{
    if (pin > 15 || !gpio_port) {
        return ERROR_INVALID;
    }

    gpio_port->OSPEEDR &= ~(3 << (pin << 1));
    gpio_port->OSPEEDR |= output_speed << (pin << 1);

    return SUCCESS;
}

static error_t gpio_set_pull_resistor(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_pull_resistor_t pull_resistor)
{
    if (pin > 15 || !gpio_port) {
        return ERROR_INVALID;
    }

    gpio_port->PUPDR &= ~(3 << (pin << 1));
    gpio_port->PUPDR |= pull_resistor << (pin << 1);

    return SUCCESS;
}

static error_t gpio_set_alternative_function(GPIO_TypeDef *gpio_port, uint8_t pin, uint8_t alternative_function)
{
    if (pin > 15 || !gpio_port) {
        return ERROR_INVALID;
    }

    if (pin < 8) {
        gpio_port->AFR[0] |= alternative_function << (pin << 2);
    } else {
        gpio_port->AFR[1] |= alternative_function << (pin << 2);
    }

    return SUCCESS;
}

static error_t gpio_invalid_handle(gpio_handle_t handle)
{
    return ((handle < 0) || (handle > CONFIG_GPIO_PIN_COUNT) || (!g_gpio_pins[handle].is_open));
}

static error_t gpio_find_free_device(gpio_handle_t *handle)
{
    error_t error = ERROR_NO_MEMORY;
    gpio_handle_t gpio_handle = 0;

    for (gpio_handle = 0; gpio_handle < CONFIG_GPIO_PIN_COUNT; gpio_handle++) {
        if (g_gpio_pins[gpio_handle].is_open) {
            continue;
        }

        error = SUCCESS;
        *handle = gpio_handle;
        break;
    }

    return error;
}

error_t gpio_read(gpio_handle_t handle, uint8_t *value)
{
    if (gpio_invalid_handle(handle) || !value) {
        return ERROR_INVALID;
    }

    GPIO_TypeDef *gpio_port = g_gpio_pins[handle].port;
    uint8_t pin = g_gpio_pins[handle].pin;

    *value = (gpio_port->IDR >> pin) & 1;
    return SUCCESS;
}

error_t gpio_write(gpio_handle_t handle, uint8_t value)
{
    if (gpio_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    GPIO_TypeDef *gpio_port = g_gpio_pins[handle].port;
    uint8_t pin = g_gpio_pins[handle].pin;

    if (value) {
        gpio_port->BSRR |= 1 << pin;
    } else {
        gpio_port->BSRR |= 1 << (pin + 16);
    }

    return SUCCESS;
}

error_t gpio_configure_pin(gpio_configuration_t config, gpio_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;

    error = gpio_find_free_device(handle);
    if (error) {
        return error;
    }

    memcpy(&g_gpio_pins[*handle], &config, sizeof(gpio_configuration_t));
    g_gpio_pins[*handle].is_open = true;

    error = gpio_set_mode(config.port, config.pin, config.mode);
    if (error) {
        return error;
    }

    error = gpio_set_output_type(config.port, config.pin, config.output_type);
    if (error) {
        return error;
    }

    error = gpio_set_output_speed(config.port, config.pin, config.output_speed);
    if (error) {
        return error;
    }

    error = gpio_set_pull_resistor(config.port, config.pin, config.pull_resistor);
    if (error) {
        return error;
    }

    error = gpio_set_alternative_function(config.port, config.pin, config.alternative_function);

    return error;
}
