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
    spi_flash_ioctl_read_id_arg_t arg;

    device_open("spi_flash", &flash_handle);
    log_init();

    log_info("Starting spi flash application");

    device_ioctl(flash_handle, SPI_FLASH_IOCTL_READ_ID, &arg);

    log_info("flash device id: %u", arg.id);

    while (1);
}
