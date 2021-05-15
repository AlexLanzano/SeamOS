#ifndef STM32WB55XX_GPIO_H
#define STM32WB55XX_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stm32wb55xx.h>
#include <libraries/error.h>
#include <mcu/gpio.h>

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

typedef enum gpio_output_speed{
    GPIO_OUTPUT_SPEED_LOW,
    GPIO_OUTPUT_SPEED_MEDIUM,
    GPIO_OUTPUT_SPEED_FAST,
    GPIO_OUTPUT_SPEED_HIGH
} gpio_output_speed_t;

typedef enum gpio_pull_resistor{
    GPIO_PULL_RESISTOR_NONE,
    GPIO_PULL_RESISTOR_UP,
    GPIO_PULL_RESISTOR_DOWN,
    GPIO_PULL_RESISTOR_RESERVED
} gpio_pull_resistor_t;

typedef struct gpio_configuration {
    bool is_open;
    GPIO_TypeDef *port;
    uint8_t pin;
    gpio_mode_t mode;
    gpio_output_type_t output_type;
    gpio_output_speed_t output_speed;
    gpio_pull_resistor_t pull_resistor;
    uint8_t alternative_function;
} gpio_configuration_t;

/* error_t gpio_set_mode(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_mode_t mode); */
/* error_t gpio_set_output_type(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_output_type_t output_type); */
/* error_t gpio_set_output_speed(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_output_speed_t output_speed); */
/* error_t gpio_set_pull_resistor(GPIO_TypeDef *gpio_port, uint8_t pin, gpio_pull_resistor_t pull_resistor); */
/* error_t gpio_set_alternative_function(GPIO_TypeDef *gpio_port, uint8_t pin, uint8_t alternative_function); */

#endif
