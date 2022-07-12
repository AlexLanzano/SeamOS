#include <config.h>
#include <mcu/interfaces/system_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/device/device.h>
#include <libraries/string.h>
#include <libraries/queue.h>
#include <libraries/error.h>
#include <libraries/log.h>
#include <applications/blink.h>

extern device_ops_t g_log_device_ops;
extern device_ops_t g_led_device_ops;

extern uint32_t g_blink_stack[];

void main()
{
    task_handle_t blink_task_handle;

    device_init("log", &g_log_device_ops, 0);
    device_init("led", &g_led_device_ops, 0);

    log_init();
    log_info("In kernel");

    task_manager_init();
    task_manager_init_task(&blink_task_entry,
                           1,
                           500,
                           g_blink_stack,
                           &blink_task_handle);

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
