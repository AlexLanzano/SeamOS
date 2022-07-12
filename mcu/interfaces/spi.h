#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include <libraries/error.h>

typedef struct spi_interface_configuration spi_interface_configuration_t;
typedef struct spi_device_configuration spi_device_configuration_t;

typedef uint32_t spi_interface_handle_t;
typedef uint32_t spi_device_handle_t;

error_t spi_device_init(spi_device_configuration_t config, spi_device_handle_t *handle);
error_t spi_device_deinit(spi_device_handle_t handle);
error_t spi_device_enable(spi_device_handle_t handle);
error_t spi_device_disable(spi_device_handle_t handle);
error_t spi_read_write(spi_device_handle_t handle, void *rdata, uint8_t wdata, uint32_t length);
error_t spi_read(spi_device_handle_t handle, void *buffer, uint32_t length);
error_t spi_read_dma(spi_device_handle_t handle, void *buffer, uint32_t length);
error_t spi_write(spi_device_handle_t handle, void *data, uint32_t length);
error_t spi_write_dma(spi_device_handle_t handle, void *data, uint32_t length);

#endif
