#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/device.h>
#include <libraries/log.h>
#include <libraries/display.h>

uint32_t g_accelerometer_task_stack[500] = {0};

void accelerometer_task_entry()
{
    error_t error;
    uint32_t accel_handle;

    log_init();
    device_open("accelerometer", &accel_handle);

    while (1) {
        uint32_t values[3];

        error = device_read(accel_handle, values, 1);
        if (error) {
            log_error(error, "Failed to read data from accelerometer");
            while(1);
        }

        log_info("X: %u  Y: %u  Z: %u", values[0], values[1], values[2]);
        task_manager_task_wait_ms(1000);
    }
}
