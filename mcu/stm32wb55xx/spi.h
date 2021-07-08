#ifndef SPI_H
#define SPI_H

#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <stdbool.h>
#include <mcu/spi.h>
#include <libraries/error.h>

typedef enum spi_clock_mode {
    SPI_CLOCK_MODE_0,
    SPI_CLOCK_MODE_1,
    SPI_CLOCK_MODE_3,
    SPI_CLOCK_MODE_4
} spi_clock_mode_t;

typedef enum spi_mode {
    SPI_MODE_SLAVE,
    SPI_MODE_MASTER
} spi_mode_t;

typedef enum spi_baud_rate_prescaler {
    SPI_BAUD_RATE_PRESCALER_2,
    SPI_BAUD_RATE_PRESCALER_4,
    SPI_BAUD_RATE_PRESCALER_8,
    SPI_BAUD_RATE_PRESCALER_16,
    SPI_BAUD_RATE_PRESCALER_32,
    SPI_BAUD_RATE_PRESCALER_64,
    SPI_BAUD_RATE_PRESCALER_128,
    SPI_BAUD_RATE_PRESCALER_256
} spi_baud_rate_prescaler_t;

typedef enum spi_significant_bit {
    SPI_SIGNIFICANT_BIT_MSB,
    SPI_SIGNIFICANT_BIT_LSB,
} spi_significant_bit_t;

typedef enum spi_com_mode {
    SPI_COM_MODE_FULL_DUPLEX,
    SPI_COM_MODE_HALF_DUPLEX_RECEIVE,
    SPI_COM_MODE_HALF_DUPLEX_TRANSMIT,
    SPI_COM_MODE_SIMPLEX
} spi_com_mode_t;

typedef enum spi_data_size {
    SPI_DATA_SIZE_4BIT = 3,
    SPI_DATA_SIZE_5BIT,
    SPI_DATA_SIZE_6BIT,
    SPI_DATA_SIZE_7BIT,
    SPI_DATA_SIZE_8BIT,
    SPI_DATA_SIZE_9BIT,
    SPI_DATA_SIZE_10BIT,
    SPI_DATA_SIZE_11BIT,
    SPI_DATA_SIZE_12BIT,
    SPI_DATA_SIZE_13BIT,
    SPI_DATA_SIZE_14BIT,
    SPI_DATA_SIZE_15BIT,
    SPI_DATA_SIZE_16BIT
} spi_data_size_t;

typedef struct spi_interface_configuration {
    GPIO_TypeDef *sck_port;
    uint8_t sck_pin;

    GPIO_TypeDef *miso_port;
    uint8_t miso_pin;

    GPIO_TypeDef *mosi_port;
    uint8_t mosi_pin;
} spi_interface_configuration_t;

typedef struct spi_device_configuration {
    SPI_TypeDef *spi;
    GPIO_TypeDef *cs_port;
    uint8_t cs_pin;
    spi_clock_mode_t clock_mode;
    spi_mode_t mode;
    spi_baud_rate_prescaler_t baud_rate_prescaler;
    spi_significant_bit_t significant_bit;
    spi_com_mode_t com_mode;
    spi_data_size_t data_size;
    bool active_low;
} spi_device_configuration_t;

#endif
