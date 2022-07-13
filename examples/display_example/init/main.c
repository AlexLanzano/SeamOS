#include <config.h>
#include <kernel/task/task_manager.h>
#include <kernel/device/device.h>
#include <drivers/serial/log.h>
#include <drivers/display/st7789.h>
#include <libraries/error.h>
#include <libraries/log.h>

extern device_ops_t g_log_device_ops;
static log_configuration_t g_log_config = {
    .interface_handle = 0,
};

extern device_ops_t g_st7789_device_ops;
uint16_t framebuffer[57600] = {0};
static st7789_configuration_t g_st7789_config = {
    .interface_handle = 0,
    .cs_pin = 8,
    .dc_pin = 9,
    .width = 240,
    .height = 240,
    .framebuffer = framebuffer,
    .color_format = ST7789_COLOR_FORMAT_16_BIT
};

extern uint32_t g_display_task_stack[];
extern void display_task_entry();


void main()
{
    task_handle_t display_task_handle;

    device_init("log", &g_log_device_ops, &g_log_config);
    device_init("display", &g_st7789_device_ops, &g_st7789_config);

    //log_init();
    log_info("Display Example!");


    task_manager_init();
    task_manager_init_task(&display_task_entry,
                           1,
                           500,
                           g_display_task_stack,
                           &display_task_handle);
    task_manager_start();
}

void HardFault_Handler()
{
    log_error(ERROR_FAULT, "HARD FAULT");
    while(1);
}

void BusFault_Handler()
{
    log_error(ERROR_FAULT, "BUS FAULT");
    while(1);
}

void UsageFault_Handler()
{
    log_error(ERROR_FAULT, "USEAGE FAULT");
    while(1);
}

/* void dma_transfer_complete_handler(uint32_t handle) */
/* { */
/*     dma_stop(handle); */
/*     DMA1->IFCR |= 0xfffffff; */
/* } */
