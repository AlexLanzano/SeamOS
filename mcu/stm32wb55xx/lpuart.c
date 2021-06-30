#include <stdint.h>
#include <stdbool.h>
#include <stm32wb55xx.h>
#include <stm32wb55xx/lpuart.h>
#include <stm32wb55xx/rcc.h>
#include <stm32wb55xx/gpio.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <libraries/semaphore.h>

#define LPUART_DEVICE_MAX 8

typedef struct lpuart_device {
    bool is_initialized;
    USART_TypeDef *device;
} lpuart_device_t;

static lpuart_device_t g_lpuart_devices[LPUART_DEVICE_MAX];
static semaphore_handle_t g_lpuart_mutex;

static bool lpuart_invalid_handle(lpuart_handle_t handle)
{
    return (handle >= LPUART_DEVICE_MAX || !g_lpuart_devices[handle].is_initialized);
}

static error_t lpuart_find_free_device(lpuart_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    int32_t lpuart_handle;

    for (lpuart_handle = 0; lpuart_handle < LPUART_DEVICE_MAX; lpuart_handle++) {
        if (!g_lpuart_devices[lpuart_handle].is_initialized) {
            *handle = lpuart_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

error_t lpuart_read(lpuart_handle_t handle, uint8_t *data)
{
    if (lpuart_invalid_handle(handle) || !data) {
        return ERROR_INVALID;
    }

    USART_TypeDef *device = g_lpuart_devices[handle].device;

    while (semaphore_lock(g_lpuart_mutex) == ERROR_LOCKED);

    *data = device->RDR;

    semaphore_release(g_lpuart_mutex);

    return SUCCESS;
}

error_t lpuart_write(lpuart_handle_t handle, uint8_t *data, uint32_t length)
{
    if (lpuart_invalid_handle(handle) || !data || !length) {
        return ERROR_INVALID;
    }

    USART_TypeDef *device = g_lpuart_devices[handle].device;

    while (semaphore_lock(g_lpuart_mutex) == ERROR_LOCKED);

    for (uint32_t i = 0; i < length; i++) {
        device->TDR = (uint32_t)data[i];
        // TODO: add timeout here
        while (!(device->ISR & USART_ISR_TC)) {}
    }

    semaphore_release(g_lpuart_mutex);

    return SUCCESS;
}

error_t lpuart_init(lpuart_configuration_t config, lpuart_handle_t *handle)
{
    error_t error;
    gpio_handle_t tx_pin_handle;
    gpio_configuration_t tx_pin;
    gpio_handle_t rx_pin_handle;
    gpio_configuration_t rx_pin;

    error = lpuart_find_free_device(handle);
    if (error) {
        return error;
    }

    g_lpuart_devices[*handle].is_initialized = true;
    g_lpuart_devices[*handle].device = config.lpuart;

    error = semaphore_init(1, &g_lpuart_mutex);
    if (error) {
        return error;
    }

    tx_pin.port = config.tx_port;
    tx_pin.pin = config.tx_pin;
    tx_pin.mode = GPIO_MODE_ALT_FUNC;
    tx_pin.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    tx_pin.output_speed = GPIO_OUTPUT_SPEED_FAST;
    tx_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    tx_pin.alternative_function = 8;
    error = gpio_configure_pin(tx_pin, &tx_pin_handle);
    if (error) {
        return error;
    }

    rx_pin.port = config.rx_port;
    rx_pin.pin = config.rx_pin;
    rx_pin.mode = GPIO_MODE_ALT_FUNC;
    rx_pin.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    rx_pin.output_speed = GPIO_OUTPUT_SPEED_FAST;
    rx_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    rx_pin.alternative_function = 8;
    error = gpio_configure_pin(rx_pin, &rx_pin_handle);
    if (error) {
        return error;
    }

    rcc_set_lpuart1_clock_source(config.clock_source);
    config.lpuart->CR1 |= config.word_length << USART_CR1_M1_Pos;
    config.lpuart->BRR = config.baud_rate_prescaler;

    // Enable LPUART
    config.lpuart->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    return SUCCESS;
}
