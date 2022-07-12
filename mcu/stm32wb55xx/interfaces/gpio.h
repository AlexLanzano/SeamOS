#ifndef STM32WB55XX_GPIO_H
#define STM32WB55XX_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <libraries/error.h>
#include <mcu/interfaces/gpio.h>

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

typedef struct gpio_configuration {
    gpio_output_type_t output_type;
    gpio_output_speed_t output_speed;
    uint8_t alternative_function;
} gpio_configuration_t;

typedef struct gpio {
    uint8_t pin;
    GPIO_TypeDef *port;
} gpio_t;

#endif
