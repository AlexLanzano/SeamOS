#include <config.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <stdint.h>
#include <stdbool.h>

static gpio_t g_pin_map[] = CONFIG_PIN_MAP;

// TODO: Implement pin number validation

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
        gpio_port->AFR[1] |= alternative_function << ((pin-8) << 2);
    }

    return SUCCESS;
}

error_t gpio_init(uint32_t pin_number, gpio_configuration_t *config)
{
    error_t error;
    uint8_t pin = g_pin_map[pin_number].pin;
    GPIO_TypeDef *port = g_pin_map[pin_number].port;

    error = gpio_set_mode(port, pin, config->mode);
    if (error) {
        return error;
    }

    error = gpio_set_pull_resistor(port, pin, config->pull_resistor);
    if (error) {
        return error;
    }

    error = gpio_set_output_type(port, pin, config->output_type);
    if (error) {
        return error;
    }

    error = gpio_set_output_speed(port, pin, CONFIG_GPIO_SPEED);
    if (error) {
        return error;
    }

    error = gpio_set_alternative_function(port, pin, config->alternate_function);
    return error;
}

error_t gpio_deinit(uint32_t pin_number)
{
    // TODO: Implement
    return SUCCESS;
}

error_t gpio_read(uint32_t pin_number, uint8_t *value)
{
    if (!value) {
        return ERROR_INVALID;
    }

    GPIO_TypeDef *gpio_port = g_pin_map[pin_number].port;
    uint8_t pin = g_pin_map[pin_number].pin;

    *value = (gpio_port->IDR >> pin) & 1;
    return SUCCESS;
}

error_t gpio_write(uint32_t pin_number, uint8_t value)
{
    GPIO_TypeDef *gpio_port = g_pin_map[pin_number].port;
    uint8_t pin = g_pin_map[pin_number].pin;

    if (value) {
        gpio_port->BSRR |= 1 << pin;
    } else {
        gpio_port->BSRR |= 1 << (pin + 16);
    }

    return SUCCESS;
}
