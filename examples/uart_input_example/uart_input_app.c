#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/device.h>
#include <libraries/log.h>
#include <drivers/memory/spi_flash.h>

uint32_t g_uart_input_task_stack[500];

void uart_input_task_entry()
{
    uint32_t uart_handle;

    device_open("log", &uart_handle);

    device_write(uart_handle, "TEST\r\n", 6);

    while (1) {
        char c = 0;

        device_read(uart_handle, &c, 1);
        device_write(uart_handle, &c, 1);
        device_write(uart_handle, "\r\n", 2);
    }
}
