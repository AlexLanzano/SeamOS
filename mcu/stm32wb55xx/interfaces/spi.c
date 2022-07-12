#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/interfaces/spi.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <mcu/stm32wb55xx/interfaces/dma.h>


#ifndef CONFIG_SPI_DEVICE_MAX
#define CONFIG_SPI_DEVICE_MAX 8
#endif

#ifndef CONFIG_SPI_INTERFACE_MAX
#define CONFIG_SPI_INTERFACE_MAX 2
#endif

typedef struct spi_device {
    bool is_initialized;
    spi_device_configuration_t config;
} spi_device_t;

spi_device_t g_spi_devices[CONFIG_SPI_DEVICE_MAX];
spi_device_handle_t g_current_spi_device = -1;

static bool spi_invalid_handle(spi_device_handle_t handle)
{
    return (handle >= CONFIG_SPI_DEVICE_MAX || !g_spi_devices[handle].is_initialized);
}

static error_t spi_configure(spi_device_handle_t handle)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    spi_device_configuration_t *spi_dev = &g_spi_devices[handle].config;
    g_current_spi_device = handle;

    uint32_t cr1_value = 0;
    uint32_t cr2_value = 0;

    cr1_value |=
        (spi_dev->clock_mode << SPI_CR1_CPHA_Pos) |
        (spi_dev->mode << SPI_CR1_MSTR_Pos) |
        (spi_dev->baud_rate_prescaler << SPI_CR1_BR_Pos) |
        (spi_dev->significant_bit << SPI_CR1_LSBFIRST_Pos);

    if (spi_dev->com_mode == SPI_COM_MODE_FULL_DUPLEX) {
        cr1_value &= ~(1 << SPI_CR1_RXONLY_Pos);

    } else if (spi_dev->com_mode == SPI_COM_MODE_HALF_DUPLEX_RECEIVE) {
        cr1_value |= 1 << SPI_CR1_BIDIMODE_Pos;
        cr1_value &= ~(1 << SPI_CR1_BIDIOE_Pos);

    } else if (spi_dev->com_mode == SPI_COM_MODE_HALF_DUPLEX_TRANSMIT) {
        cr1_value |= (1 << SPI_CR1_BIDIMODE_Pos) |
            (1 << SPI_CR1_BIDIOE_Pos);
    }

    cr1_value |= SPI_CR1_SSM;
    cr2_value |= SPI_CR2_SSOE;

    cr2_value |= spi_dev->data_size << SPI_CR2_DS_Pos;
    cr2_value |= SPI_CR2_FRXTH;

    spi_dev->spi->CR1 = cr1_value;
    spi_dev->spi->CR2 = cr2_value;

    spi_dev->spi->CR1 |= SPI_CR1_SPE;

    return SUCCESS;
}

static bool spi_tx_buffer_empty(SPI_TypeDef *spi)
{
    return (bool)(spi->SR & 2);
}

static void spi_transmit_8bit(SPI_TypeDef *spi, uint8_t data)
{
    while (!spi_tx_buffer_empty(spi)) {}
    *(uint8_t *)&spi->DR = data;
}

static void spi_receive_8bit(SPI_TypeDef *spi, uint8_t *data)
{
    *data = spi->DR;
}

static error_t spi_find_free_device(spi_device_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    spi_device_handle_t spi_handle;

    for (spi_handle = 0; spi_handle < CONFIG_SPI_DEVICE_MAX; spi_handle++) {
        if (!g_spi_devices[spi_handle].is_initialized) {
            *handle = spi_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

error_t spi_device_enable(spi_device_handle_t handle)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    spi_device_t *spi_device = &g_spi_devices[handle];

    if (spi_device->config.active_low) {
        gpio_write(spi_device->config.cs_pin, 0);
    } else {
        gpio_write(spi_device->config.cs_pin, 1);
    }

    return SUCCESS;
}

error_t spi_device_disable(spi_device_handle_t handle)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    spi_device_t *spi_device = &g_spi_devices[handle];

    if (spi_device->config.active_low) {
        gpio_write(spi_device->config.cs_pin, 1);
    } else {
        gpio_write(spi_device->config.cs_pin, 0);
    }

    return SUCCESS;
}

error_t spi_read_write(spi_device_handle_t handle, void *rdata, uint8_t wdata, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    if (g_current_spi_device != handle) {
        spi_configure(handle);
    }

    SPI_TypeDef *spi = g_spi_devices[handle].config.spi;
    uint8_t *rd = rdata;
    while (length--) {
        spi_transmit_8bit(spi, wdata);
        spi_receive_8bit(spi, rd++);
    }

    return SUCCESS;
}

error_t spi_read(spi_device_handle_t handle, void *buffer, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    if (g_current_spi_device != handle) {
        spi_configure(handle);
    }

    SPI_TypeDef *spi = g_spi_devices[handle].config.spi;
    uint8_t *d = buffer;
    while (length--) {
        spi_receive_8bit(spi, d++);
    }

    return SUCCESS;
}

error_t spi_read_dma(spi_device_handle_t handle, void *buffer, uint32_t length)
{
    // TODO: Implement
    return SUCCESS;
}

error_t spi_write(spi_device_handle_t handle, void *data, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    if (g_current_spi_device != handle) {
        spi_configure(handle);
    }

    SPI_TypeDef *spi = g_spi_devices[handle].config.spi;
    uint8_t *d = data;
    while (length--) {
        while (!spi_tx_buffer_empty(spi)) {}
        *(uint8_t *)&spi->DR = *d++;
    }

    return SUCCESS;
}

error_t spi_write_dma(spi_device_handle_t handle, void *data, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    spi_device_configuration_t config = g_spi_devices[handle].config;

    dma_start(config.dma_handle, (uint32_t)&config.spi->DR, (uint32_t)data, length);
    config.spi->CR2 |= SPI_CR2_TXDMAEN;

    return SUCCESS;
}

error_t spi_device_init(spi_device_configuration_t config, spi_device_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;

    error = spi_find_free_device(handle);
    if (error) {
        return error;
    }

    memcpy(&g_spi_devices[*handle].config, &config, sizeof(spi_device_configuration_t));

    error = gpio_init((gpio_configuration_t)
                      {.mode = GPIO_MODE_OUTPUT,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_LOW,
                       .pull_resistor = GPIO_PULL_RESISTOR_NONE},
                      config.cs_pin);
    if (error) {
        return error;
    }

    g_spi_devices[*handle].is_initialized = true;
    spi_configure(*handle);

    return SUCCESS;
}

error_t spi_device_deinit(spi_device_handle_t handle)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    g_spi_devices[handle].is_initialized = false;

    return SUCCESS;
}

error_t spi_interface_init(SPI_TypeDef *spi, uint32_t sck_pin, uint32_t miso_pin, uint32_t mosi_pin)
{
    error_t error;
    gpio_configuration_t sck_pin_config = {0};
    gpio_configuration_t miso_pin_config = {0};
    gpio_configuration_t mosi_pin_config = {0};

    sck_pin_config.mode = GPIO_MODE_ALT_FUNC;
    sck_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    sck_pin_config.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    sck_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    sck_pin_config.alternative_function = 5;
    error = gpio_init(sck_pin_config, sck_pin);
    if (error) {
        log_error(error, "Failed to configure sck pin");
        return error;
    }

    miso_pin_config.mode = GPIO_MODE_ALT_FUNC;
    miso_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    miso_pin_config.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    miso_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    miso_pin_config.alternative_function = 5;
    error = gpio_init(miso_pin_config, miso_pin);
    if (error) {
        log_error(error, "Failed to configure miso pin");
        return error;
    }

    mosi_pin_config.mode = GPIO_MODE_ALT_FUNC;
    mosi_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    mosi_pin_config.output_speed = GPIO_OUTPUT_SPEED_HIGH;
    mosi_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    mosi_pin_config.alternative_function = 5;
    error = gpio_init(mosi_pin_config, mosi_pin);
    if (error) {
        log_error(error, "Failed to configure mosi pin");
        return error;
    }

    return SUCCESS;
}
