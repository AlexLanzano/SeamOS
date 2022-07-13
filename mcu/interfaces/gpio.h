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

typedef enum gpio_output_type{
    GPIO_OUTPUT_TYPE_PUSH_PULL,
    GPIO_OUTPUT_TYPE_OPEN_DRAIN
} gpio_output_type_t;

typedef enum gpio_pull_resistor{
    GPIO_PULL_RESISTOR_NONE,
    GPIO_PULL_RESISTOR_UP,
    GPIO_PULL_RESISTOR_DOWN,
} gpio_pull_resistor_t;

typedef struct gpio_configuration {
    gpio_mode_t mode;
    gpio_mode_t output_type;
    gpio_pull_resistor_t pull_resistor;
    uint32_t alternate_function;
    void *mcu_config;
} gpio_configuration_t;

error_t gpio_init(uint32_t pin, gpio_configuration_t *config);
error_t gpio_deinit(uint32_t pin);
error_t gpio_read(uint32_t pin, uint8_t *value);
error_t gpio_write(uint32_t pin, uint8_t value);

#endif
