#include <stdint.h>
#include <stdbool.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/spi.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <mcu/stm32wb55xx/gpio.h>

#define SPI_DEVICE_MAX 8

typedef struct spi_device {
    bool is_initialized;
    spi_device_configuration_t device;
} spi_device_t;

spi_device_t g_spi_devices[SPI_DEVICE_MAX];
spi_handle_t g_current_spi_device = -1;

static bool spi_invalid_handle(spi_handle_t handle)
{
    return (handle >= SPI_DEVICE_MAX || !g_spi_devices[handle].is_initialized);
}

static error_t spi_configure(spi_handle_t handle)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    spi_device_configuration_t *spi_dev = &g_spi_devices[handle].device;
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

static error_t spi_find_free_device(spi_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    spi_handle_t spi_handle;

    for (spi_handle = 0; spi_handle < SPI_DEVICE_MAX; spi_handle++) {
        if (!g_spi_devices[spi_handle].is_initialized) {
            *handle = spi_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

error_t spi_read_write(spi_handle_t handle, void *rdata, uint8_t wdata, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    if (g_current_spi_device != handle) {
        spi_configure(handle);
    }

    SPI_TypeDef *spi = g_spi_devices[handle].device.spi;
    uint8_t *rd = rdata;
    while (length--) {
        spi_transmit_8bit(spi, wdata);
        spi_receive_8bit(spi, rd++);
    }

    return SUCCESS;
}

error_t spi_read(spi_handle_t handle, void *buffer, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    if (g_current_spi_device != handle) {
        spi_configure(handle);
    }

    SPI_TypeDef *spi = g_spi_devices[handle].device.spi;
    uint8_t *d = buffer;
    while (length--) {
        spi_receive_8bit(spi, d++);
    }

    return SUCCESS;
}

error_t spi_write(spi_handle_t handle, void *data, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    if (g_current_spi_device != handle) {
        spi_configure(handle);
    }

    SPI_TypeDef *spi = g_spi_devices[handle].device.spi;
    uint8_t *d = data;
    while (length--) {
        spi_transmit_8bit(spi, *d++);
    }

    return SUCCESS;
}

error_t spi_device_init(spi_device_configuration_t config, spi_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;

    error = spi_find_free_device(handle);
    if (error) {
        return error;
    }

    memcpy(&g_spi_devices[*handle].device, &config, sizeof(spi_device_configuration_t));
    g_spi_devices[*handle].is_initialized = true;
    spi_configure(*handle);

    return SUCCESS;
}

error_t spi_device_deinit(spi_handle_t handle)
{
    if (spi_invalid_handle(handle)) {
        log_error(ERROR_INVALID, "Invalid SPI handle");
        return ERROR_INVALID;
    }

    g_spi_devices[handle].is_initialized = false;

    return SUCCESS;
}

error_t spi_interface_init(spi_interface_configuration_t config)
{
    error_t error;
    gpio_configuration_t sck_pin;
    gpio_configuration_t miso_pin;
    gpio_configuration_t mosi_pin;

    sck_pin.port = config.sck_port;
    sck_pin.pin = config.sck_pin;
    sck_pin.mode = GPIO_MODE_ALT_FUNC;
    sck_pin.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    sck_pin.output_speed = GPIO_OUTPUT_SPEED_LOW;
    sck_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    sck_pin.alternative_function = 5;
    error = gpio_configure_pin(sck_pin, &config.sck_handle);
    if (error) {
        log_error(error, "Failed to configure sck pin");
        return error;
    }

    miso_pin.port = config.miso_port;
    miso_pin.pin = config.miso_pin;
    miso_pin.mode = GPIO_MODE_ALT_FUNC;
    miso_pin.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    miso_pin.output_speed = GPIO_OUTPUT_SPEED_LOW;
    miso_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    miso_pin.alternative_function = 5;
    error = gpio_configure_pin(miso_pin, &config.miso_handle);
    if (error) {
        log_error(error, "Failed to configure miso pin");
        return error;
    }

    mosi_pin.port = config.mosi_port;
    mosi_pin.pin = config.mosi_pin;
    mosi_pin.mode = GPIO_MODE_ALT_FUNC;
    mosi_pin.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    mosi_pin.output_speed = GPIO_OUTPUT_SPEED_LOW;
    mosi_pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    mosi_pin.alternative_function = 5;
    error = gpio_configure_pin(mosi_pin, &config.mosi_handle);
    if (error) {
        log_error(error, "Failed to configure mosi pin");
        return error;
    }

    return SUCCESS;
}
