#ifndef STM32WB55XX_LPUART_H
#define STM32WB55XX_LPUART_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/lpuart.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <mcu/stm32wb55xx/gpio.h>

typedef enum lpuart_word_length {
    LPUART_WORD_LENGTH_8,
    LPUART_WORD_LENGTH_9,
    LPUART_WORD_LENGTH_7
} lpuart_word_length_t;

typedef enum lpuart_stop_bits {
    LPUART_STOP_BITS_1 = 0,
    LPUART_STOP_BITS_2 = 2
} lpuart_stop_bits_t;

typedef struct lpuart_configuration {
    USART_TypeDef *lpuart;

    GPIO_TypeDef *rx_port;
    uint8_t rx_pin;
    GPIO_TypeDef *tx_port;
    uint8_t tx_pin;

    rcc_lpuart_clock_source_t clock_source;
    lpuart_word_length_t word_length;
    uint32_t baud_rate_prescaler;
    lpuart_stop_bits_t stop_bits;
} lpuart_configuration_t;

#endif
