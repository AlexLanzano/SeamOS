#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <libraries/error.h>

typedef uint32_t gpio_handle_t;
typedef struct gpio_configuration  gpio_configuration_t;

typedef enum gpio_mode{
    GPIO_MODE_INPUT,
    GPIO_MODE_OUTPUT,
    GPIO_MODE_ALT_FUNC,
    GPIO_MODE_ANALOG,
} gpio_mode_t;

typedef enum gpio_pull_resistor{
    GPIO_PULL_RESISTOR_NONE,
    GPIO_PULL_RESISTOR_UP,
    GPIO_PULL_RESISTOR_DOWN,
} gpio_pull_resistor_t;

typedef struct gpio_configuration gpio_configuration_t;
typedef struct gpio_interface_configuration {
    gpio_mode_t mode;
    gpio_pull_resistor_t pull_resistor;
    void *mcu_config;
} gpio_interface_configuration_t;

error_t gpio_init(uint32_t pin, gpio_interface_configuration_t *config);
error_t gpio_deinit(uint32_t pin);
error_t gpio_read(uint32_t pin, uint8_t *value);
error_t gpio_write(uint32_t pin, uint8_t value);

#endif
