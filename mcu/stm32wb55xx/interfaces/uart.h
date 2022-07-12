#ifndef STM32WB55XX_UART_H
#define STM32WB55XX_UART_H

#include <stdint.h>
#include <libraries/error.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>

typedef enum uart_word_length {
    UART_WORD_LENGTH_8,
    UART_WORD_LENGTH_9,
    UART_WORD_LENGTH_7
} uart_word_length_t;

typedef enum uart_stop_bits {
    UART_STOP_BITS_1 = 0,
    UART_STOP_BITS_2 = 2
} uart_stop_bits_t;

typedef struct uart_configuration {
    uart_word_length_t word_length;
    uart_stop_bits_t stop_bits;
} uart_configuration_t;

error_t uart_enable(USART_TypeDef *uart, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud, uint32_t word_length);
error_t uart_disable(USART_TypeDef *uart, uint32_t tx_pin);

#endif
