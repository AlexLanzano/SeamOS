#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <libraries/error.h>

typedef enum spi_clock_mode {
    SPI_CLOCK_MODE_0,
    SPI_CLOCK_MODE_1,
    SPI_CLOCK_MODE_2,
    SPI_CLOCK_MODE_3
} spi_clock_mode_t;

typedef enum spi_mode {
    SPI_MODE_SLAVE,
    SPI_MODE_MASTER
} spi_mode_t;

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

typedef struct spi_configuration {
    spi_mode_t mode;
    spi_clock_mode_t clock_mode;
    spi_significant_bit_t significant_bit;
    spi_com_mode_t com_mode;
    spi_data_size_t data_size;
} spi_configuration_t;

// error_t spi_init(uint32_t handle, spi_configuration_t *config);
error_t spi_read(uint32_t handle, spi_configuration_t *config, uint8_t *data, uint32_t length);
error_t spi_write(uint32_t handle, spi_configuration_t *config, uint8_t *data, uint32_t length);
error_t spi_read_write(uint32_t handle, spi_configuration_t *config, uint8_t *rdata, uint8_t wdata, uint32_t length);
error_t spi_read_dma(uint32_t spi_handle, spi_configuration_t *config, uint8_t *buffer, uint32_t length);
error_t spi_write_dma(uint32_t spi_handle, spi_configuration_t *config, uint8_t *data, uint32_t length);

#endif
