#ifndef STM32WB55XX_SPI_H
#define STM32WB55XX_SPI_H

#include <stdbool.h>
#include <libraries/error.h>

typedef struct spi_interface_configuration spi_interface_configuration_t;
typedef struct spi_device_configuration spi_device_configuration_t;

typedef uint32_t spi_handle_t;

error_t spi_read_write(spi_handle_t handle, void *rdata, uint8_t wdata, uint32_t length);
error_t spi_read(spi_handle_t handle, void *buffer, uint32_t length);
error_t spi_write(spi_handle_t handle, void *data, uint32_t length);
error_t spi_device_init(spi_device_configuration_t config, spi_handle_t *spi_handle);
error_t spi_close(spi_handle_t handle);
error_t spi_interface_init(spi_interface_configuration_t config);

#endif