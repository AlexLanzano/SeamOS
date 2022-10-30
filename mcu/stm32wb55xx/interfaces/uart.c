#include <stdint.h>
#include <stdbool.h>
#include <config.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/interfaces/uart.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <mcu/interfaces/gpio.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <libraries/semaphore.h>

#define MAX_UART_INTERFACES 1

semaphore_handle_t g_uart_mutexes[MAX_UART_INTERFACES] = {0};
USART_TypeDef *g_uart_interfaces[MAX_UART_INTERFACES] = {LPUART1};

static bool uart_invalid_handle(uint32_t handle)
{
    return (handle >= MAX_UART_INTERFACES);
}

static uint32_t uart_get_baud_prescaler(uint32_t baud)
{
    return (CONFIG_CLOCK_FREQ/baud)*256;
}

error_t uart_enable(USART_TypeDef *uart, uint32_t tx_pin, uint32_t rx_pin, uint32_t baud, uint32_t word_length)
{
    error_t error;

    gpio_configuration_t tx_pin_config = {0};
    gpio_configuration_t rx_pin_config = {0};

    tx_pin_config.mode = GPIO_MODE_ALT_FUNC;
    tx_pin_config.pull_resistor = GPIO_PULL_RESISTOR_UP;
    tx_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    if (uart == LPUART1) {
        tx_pin_config.alternate_function = GPIO_ALT_FUNC_8_LPUART;
    } else {
        tx_pin_config.alternate_function = GPIO_ALT_FUNC_7_UART;
    }
    error = gpio_init(tx_pin, &tx_pin_config);
    if (error) {
        return error;
    }

    rx_pin_config.mode = GPIO_MODE_ALT_FUNC;
    rx_pin_config.pull_resistor = GPIO_PULL_RESISTOR_UP;
    rx_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    if (uart == LPUART1) {
        rx_pin_config.alternate_function = GPIO_ALT_FUNC_8_LPUART;
    } else {
        rx_pin_config.alternate_function = GPIO_ALT_FUNC_7_UART;
    }
    error = gpio_init(rx_pin, &rx_pin_config);
    if (error) {
        return error;
    }

    uart->CR1 |= word_length << USART_CR1_M1_Pos;
    uart->BRR = uart_get_baud_prescaler(baud);

    // Enable UART
    uart->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    return SUCCESS;
}

error_t uart_disable(USART_TypeDef *uart, uint32_t tx_pin)
{
    // TODO: Implement
    return SUCCESS;
}

error_t uart_init(uint32_t handle, void *config)
{
    (void)config;
    if (uart_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    semaphore_init(1, &g_uart_mutexes[handle]);

    return SUCCESS;
}

error_t uart_deinit(uint32_t handle)
{
    if (uart_invalid_handle(handle)) {
        return ERROR_INVALID;
    }
    semaphore_deinit(g_uart_mutexes[handle]);
    return SUCCESS;
}

error_t uart_read(uint32_t handle, uint8_t *data, uint32_t length)
{
    if (uart_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    USART_TypeDef *device = g_uart_interfaces[handle];

    while (semaphore_lock(g_uart_mutexes[handle]) == ERROR_LOCKED);

    for (uint32_t i = 0; i < length; i++) {
        while (!(device->ISR & USART_ISR_RXNE));
        data[i] = device->RDR;
    }

    semaphore_release(g_uart_mutexes[handle]);

    return SUCCESS;
}

error_t uart_write(uint32_t handle, uint8_t *data, uint32_t length)
{
    if (uart_invalid_handle(handle) || !data || !length) {
        return ERROR_INVALID;
    }

    USART_TypeDef *device = g_uart_interfaces[handle];

    while (semaphore_lock(g_uart_mutexes[handle]) == ERROR_LOCKED);

    for (uint32_t i = 0; i < length; i++) {
        device->TDR = (uint32_t)data[i];
        // TODO: add timeout here
        while (!(device->ISR & USART_ISR_TC)) {}
    }

    semaphore_release(g_uart_mutexes[handle]);

    return SUCCESS;
}
