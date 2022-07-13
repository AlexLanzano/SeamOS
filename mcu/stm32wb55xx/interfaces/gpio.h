#ifndef STM32WB55XX_GPIO_H
#define STM32WB55XX_GPIO_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <libraries/error.h>
#include <mcu/interfaces/gpio.h>

typedef enum gpio_output_speed{
    GPIO_OUTPUT_SPEED_LOW,
    GPIO_OUTPUT_SPEED_MEDIUM,
    GPIO_OUTPUT_SPEED_FAST,
    GPIO_OUTPUT_SPEED_HIGH
} gpio_output_speed_t;

typedef enum gpio_alt_func {
    GPIO_ALT_FUNC_0_SYS_AF,
    GPIO_ALT_FUNC_1_TIM,
    GPIO_ALT_FUNC_2_TIM,
    GPIO_ALT_FUNC_3_SPI_SAI,
    GPIO_ALT_FUNC_4_I2C,
    GPIO_ALT_FUNC_5_SPI,
    GPIO_ALT_FUNC_6_RF,
    GPIO_ALT_FUNC_7_UART,
    GPIO_ALT_FUNC_8_LPUART,
    GPIO_ALT_FUNC_9_TSC,
    GPIO_ALT_FUNC_10_USB_QSPI,
    GPIO_ALT_FUNC_11_LCD,
    GPIO_ALT_FUNC_12_COMP_TIM,
    GPIO_ALT_FUNC_13_SAI,
    GPIO_ALT_FUNC_14_TIM,
    GPIO_ALT_FUNC_15_EVENTOUT
} gpio_alt_func_t;

typedef struct gpio {
    uint8_t pin;
    GPIO_TypeDef *port;
} gpio_t;

#endif
