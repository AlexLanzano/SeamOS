#include <stdint.h>
#include <mcu/interfaces/gpio.h>
#include <mcu/interfaces/spi.h>
#include <mcu/interfaces/system_timer.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <libraries/log.h>
#include <kernel/device/device.h>
#include <drivers/memory/spi_flash.h>

typedef enum spi_flash_command {
    SPI_FLASH_COMMAND_WRITE = 0x2,
    SPI_FLASH_COMMAND_READ = 0x3,
    SPI_FLASH_COMMAND_READ_ID = 0x9F
} spi_flash_command_t;

static gpio_configuration_t g_cs_pin_config = {
    .mode = GPIO_MODE_OUTPUT,
    .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
    .pull_resistor = GPIO_PULL_RESISTOR_NONE
};

static spi_configuration_t g_spi_interface_config = {
    .mode = SPI_MODE_MASTER,
    .clock_mode = SPI_CLOCK_MODE_0,
    .significant_bit = SPI_SIGNIFICANT_BIT_MSB,
    .com_mode = SPI_COM_MODE_FULL_DUPLEX,
    .data_size = SPI_DATA_SIZE_8BIT
};

static void spi_flash_enable(uint32_t cs_pin)
{
    gpio_write(cs_pin, 0);
}

static void spi_flash_disable(uint32_t cs_pin)
{
    gpio_write(cs_pin, 1);
}

error_t spi_flash_init(void *config)
{
    if (!config) {
        return ERROR_INVALID;
    }

    error_t error;

    spi_flash_configuration_t *device = (spi_flash_configuration_t *)config;
    error = gpio_init(device->cs_pin, &g_cs_pin_config);
    if (error) {
        return error;
    }

    spi_flash_disable(device->cs_pin);

    return SUCCESS;
}

error_t spi_flash_deinit(void *config)
{
    // TODO: Implement
    return SUCCESS;
}

error_t spi_flash_read(void *config, void *data, uint32_t data_length)
{
    return ERROR_NOT_IMPLEMENTED;
}

error_t spi_flash_write(void *config, void *data, uint32_t data_length)
{
    return ERROR_NOT_IMPLEMENTED;
}

error_t spi_flash_ioctl_read_id(const spi_flash_configuration_t *device, uint32_t *id)
{
    uint8_t command = SPI_FLASH_COMMAND_READ_ID;

    spi_flash_enable(device->cs_pin);

    spi_write(device->interface, &g_spi_interface_config, &command, 1);
    spi_read(device->interface, &g_spi_interface_config, (uint8_t *)id, 3);

    spi_flash_disable(device->cs_pin);

    return SUCCESS;
}

error_t spi_flash_ioctl_read(const spi_flash_configuration_t *device,
                             uint32_t address, void *data, uint32_t data_length)
{
    uint8_t command[4] = {SPI_FLASH_COMMAND_READ, (uint8_t)address >> 16, (uint8_t)address >> 8, (uint8_t)address};
    uint8_t *d = data;

    spi_flash_enable(device->cs_pin);

    spi_write(device->interface, &g_spi_interface_config, command, 4);
    spi_read(device->interface, &g_spi_interface_config, d, data_length);

    spi_flash_disable(device->cs_pin);

    return SUCCESS;
}

error_t spi_flash_ioctl_write(const spi_flash_configuration_t *device,
                              uint32_t address, void *data, uint32_t data_length)
{

    uint8_t command[4] = {SPI_FLASH_COMMAND_WRITE, (uint8_t)address >> 16, (uint8_t)address >> 8, (uint8_t)address};
    uint8_t *d = data;

    spi_flash_enable(device->cs_pin);
    spi_write(device->interface, &g_spi_interface_config, command, 4);

    while (data_length) {
        uint32_t size = (data_length < device->block_length) ? data_length : device->block_length;
        spi_write(device->interface, &g_spi_interface_config, d, size);
        d += size;
        data_length -= device->block_length;
    }

    spi_flash_disable(device->cs_pin);

    return SUCCESS;
}

error_t spi_flash_ioctl(void *config, uint32_t cmd, void *arg)
{
    if (!config) {
        return ERROR_INVALID;
    }

    switch (cmd) {
    case SPI_FLASH_IOCTL_READ_ID:
    {
        spi_flash_ioctl_read_id_arg_t *a = arg;
        spi_flash_ioctl_read_id(config, a->id);
        break;
    }

    case SPI_FLASH_IOCTL_READ:
    {
        spi_flash_ioctl_read_arg_t *a = arg;
        spi_flash_ioctl_read(config, a->address, a->data, a->data_length);
        break;
    }

    case SPI_FLASH_IOCTL_WRITE:
    {
        spi_flash_ioctl_write_arg_t *a = arg;
        spi_flash_ioctl_write(config, a->address, a->data, a->data_length);
        break;
    }
    }

    return SUCCESS;
}

device_ops_t g_spi_flash_device_ops = {
    .init = spi_flash_init,
    .deinit = spi_flash_deinit,
    .read = spi_flash_read,
    .write = spi_flash_write,
    .ioctl = spi_flash_ioctl
};
