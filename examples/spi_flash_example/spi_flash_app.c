#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/device.h>
#include <libraries/log.h>
#include <drivers/memory/spi_flash.h>

uint32_t g_spi_flash_task_stack[500];

void spi_flash_task_entry()
{
    uint32_t flash_handle;
    uint8_t id[3] = {0};
    spi_flash_ioctl_read_id_arg_t arg = {.id = (uint32_t *)id};


    device_open("spi_flash", &flash_handle);
    log_init();

    log_info("Starting spi flash application");

    device_ioctl(flash_handle, SPI_FLASH_IOCTL_READ_ID, &arg);

    log_info("Manufacturer id: %x", id[0]);
    log_info("Memory Type: %x", id[1]);
    log_info("Memory Capacity: %x", id[2]);

    while (1);
}
