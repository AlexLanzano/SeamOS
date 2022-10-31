#ifndef SPI_FLASH_H
#define SPI_FLASH_H

#include <stdint.h>

typedef struct spi_flash_configuration {
    uint32_t interface;
    uint32_t cs_pin;
    uint32_t block_length;
} spi_flash_configuration_t;


typedef enum spi_flash_ioctl_command {
    SPI_FLASH_IOCTL_READ_ID,
    SPI_FLASH_IOCTL_READ,
    SPI_FLASH_IOCTL_WRITE,
} spi_flash_ioctl_command_t;

typedef struct spi_flash_ioctl_read_id_arg {
    uint8_t *id;
} spi_flash_ioctl_read_id_arg_t;

typedef struct spi_flash_ioctl_read_arg {
    uint32_t address;
    void *data;
    uint32_t data_length;
} spi_flash_ioctl_read_arg_t;

typedef struct spi_flash_ioctl_write_arg {
    uint32_t address;
    void *data;
    uint32_t data_length;
} spi_flash_ioctl_write_arg_t;

#endif
