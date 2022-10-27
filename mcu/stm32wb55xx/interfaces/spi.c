#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <mcu/stm32wb55xx/interfaces/spi.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <mcu/stm32wb55xx/interfaces/dma.h>

#define MAX_SPI_INTERFACES 2
volatile int8_t g_dma_in_use =false;

static SPI_TypeDef *g_spi_interfaces[MAX_SPI_INTERFACES] = {SPI1, SPI2};

static bool spi_invalid_handle(uint32_t handle)
{
    return (handle >= MAX_SPI_INTERFACES);
}

static error_t spi_configure(SPI_TypeDef *spi, spi_configuration_t *config)
{
    uint32_t cr1_value = 0;
    uint32_t cr2_value = 0;

    cr1_value |=
        (config->clock_mode << SPI_CR1_CPHA_Pos) |
        (config->mode << SPI_CR1_MSTR_Pos) |
        (CONFIG_SPI_BAUD_RATE_PRESCALER << SPI_CR1_BR_Pos) |
        (config->significant_bit << SPI_CR1_LSBFIRST_Pos);

    if (config->com_mode == SPI_COM_MODE_FULL_DUPLEX) {
        cr1_value &= ~(1 << SPI_CR1_RXONLY_Pos);

    } else if (config->com_mode == SPI_COM_MODE_HALF_DUPLEX_RECEIVE) {
        cr1_value |= 1 << SPI_CR1_BIDIMODE_Pos;
        cr1_value &= ~(1 << SPI_CR1_BIDIOE_Pos);

    } else if (config->com_mode == SPI_COM_MODE_HALF_DUPLEX_TRANSMIT) {
        cr1_value |= (1 << SPI_CR1_BIDIMODE_Pos) |
            (1 << SPI_CR1_BIDIOE_Pos);
    }

    cr1_value |= SPI_CR1_SSM;
    cr2_value |= SPI_CR2_SSOE;

    cr2_value |= config->data_size << SPI_CR2_DS_Pos;
    cr2_value |= SPI_CR2_FRXTH;

    spi->CR1 = cr1_value;
    spi->CR2 = cr2_value;

    spi->CR1 |= SPI_CR1_SPE;

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

error_t spi_enable(SPI_TypeDef *spi, uint32_t sck_pin, uint32_t miso_pin, uint32_t mosi_pin)
{
    error_t error;
    gpio_configuration_t sck_pin_config = {0};
    gpio_configuration_t miso_pin_config = {0};
    gpio_configuration_t mosi_pin_config = {0};

    sck_pin_config.mode = GPIO_MODE_ALT_FUNC;
    sck_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    sck_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    sck_pin_config.alternate_function = GPIO_ALT_FUNC_5_SPI;
    error = gpio_init(sck_pin, &sck_pin_config);
    if (error) {
        return error;
    }

    miso_pin_config.mode = GPIO_MODE_ALT_FUNC;
    miso_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    miso_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    miso_pin_config.alternate_function = GPIO_ALT_FUNC_5_SPI;
    error = gpio_init(miso_pin, &miso_pin_config);
    if (error) {
        return error;
    }

    mosi_pin_config.mode = GPIO_MODE_ALT_FUNC;
    mosi_pin_config.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    mosi_pin_config.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    mosi_pin_config.alternate_function = GPIO_ALT_FUNC_5_SPI;
    error = gpio_init(mosi_pin, &mosi_pin_config);
    if (error) {
        return error;
    }

    return SUCCESS;
}

error_t spi_disable(uint32_t handle)
{
    // TODO: Implement
    return SUCCESS;
}

error_t spi_read(uint32_t handle, spi_configuration_t *config, uint8_t *data, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    SPI_TypeDef *spi = g_spi_interfaces[handle];
    spi_configure(spi, config);

    while (length--) {
        spi_receive_8bit(spi, data++);
    }

    return SUCCESS;
}

error_t spi_write(uint32_t handle, spi_configuration_t *config, uint8_t *data, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    SPI_TypeDef *spi = g_spi_interfaces[handle];
    spi_configure(spi,config);

    while (length--) {
        while (!spi_tx_buffer_empty(spi)) {}
        *(uint8_t *)&spi->DR = *data++;
    }

    return SUCCESS;
}

error_t spi_read_write(uint32_t handle, spi_configuration_t *config, uint8_t *rdata, uint8_t wdata, uint32_t length)
{
    if (spi_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    SPI_TypeDef *spi = g_spi_interfaces[handle];
    spi_configure(spi, config);

    while (length--) {
        spi_transmit_8bit(spi, wdata);
        spi_receive_8bit(spi, rdata++);
    }

    return SUCCESS;
}

#if defined(CONFIG_ENABLE_DMA1) || defined(CONFIG_ENABLE_DMA2)
error_t spi_read_dma(uint32_t spi_handle, spi_configuration_t *config, uint8_t *data, uint32_t length)
{
    // TODO: Implement
    return SUCCESS;
}

static void spi_write_dma_cb(uint32_t handle)
{
    SPI_TypeDef *spi = g_spi_interfaces[handle];
    spi->CR2 &= ~SPI_CR2_TXDMAEN;
    g_dma_in_use = false;
}

error_t spi_write_dma(uint32_t spi_handle, spi_configuration_t *config, uint8_t *data, uint32_t length)
{
    if (spi_invalid_handle(spi_handle)) {
        return ERROR_INVALID;
    }

    while(g_dma_in_use);
    g_dma_in_use = true;

    SPI_TypeDef *spi = g_spi_interfaces[spi_handle];
    spi_configure(spi,config);

    dma_configuration_t dma_config = {0};
    mcu_dma_configuration_t mcu_dma_config = {0};
    dma_config.mode = DMA_MODE_MEMORY_TO_INTERFACE;
    dma_config.increment_source = true;
    dma_config.increment_destination = false;
    dma_config.mcu_config = &mcu_dma_config;
    if (spi == SPI1) {
        mcu_dma_config.request_id = DMA_REQUEST_ID_SPI1_TX;
    } else if (spi == SPI2) {
        mcu_dma_config.request_id = DMA_REQUEST_ID_SPI2_TX;
    } else {
        return ERROR_INVALID;
    }

    dma_request(0, &dma_config, (void *)&spi->DR, data, length, spi_write_dma_cb, spi_handle);
    spi->CR2 |= SPI_CR2_TXDMAEN;

    return SUCCESS;
}
#endif
