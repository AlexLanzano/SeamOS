#include <config.h>
#include <kernel/task/task_manager.h>
#include <kernel/device/device.h>
#include <drivers/sensor/accelerometer/mma8451.h>
#include <drivers/serial/log.h>
#include <libraries/error.h>
#include <libraries/log.h>
#include <libraries/string.h>

extern uint32_t _estack[];
extern uint32_t _bootloader_magic[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];
extern uint32_t interrupt_vector_table[];

extern uint32_t g_accelerometer_task_stack[];
extern void accelerometer_task_entry();

extern device_ops_t g_log_device_ops;
static log_configuration_t g_log_config = {
    .interface_handle = 0,
};

extern device_ops_t g_mma8451_device_ops;
static mma8451_configuration_t g_mma8451_config = {
    .interface_handle = 0,
};

void main()
{
    task_handle_t accelerometer_task_handle;

    device_init("log", &g_log_device_ops, &g_log_config);
    device_init("accelerometer", &g_mma8451_device_ops, &g_mma8451_config);

    log_init();
    log_info("Accelerometer example!");

    task_manager_init();
    task_manager_init_task(&accelerometer_task_entry,
                           1,
                           500,
                           g_accelerometer_task_stack,
                           &accelerometer_task_handle);
    task_manager_start();
}
