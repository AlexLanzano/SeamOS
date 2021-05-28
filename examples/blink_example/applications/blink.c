#include <stm32wb55xx/gpio.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>
#include <applications/blink.h>

gpio_handle_t g_gpio_handle;
uint32_t g_initialized = 0;

static inline void turn_on_led()
{
    gpio_write(g_gpio_handle, 1);
}

static inline void turn_off_led()
{
    gpio_write(g_gpio_handle, 0);
}

void blink_application_start()
{
    log_debug("Starting blink application");
    task_t *task = task_manager_get_task_by_name(string("blink"));
    if (!g_initialized) {
        blink_application_data_t *data = (blink_application_data_t *)task->task_data;
        g_gpio_handle = data->gpio_handle;
        g_initialized = 1;
    }

    while (task_manager_task_is_running(task)) {
        log_debug("Blink!");
        turn_on_led();
        system_timer_wait_ms(500);
        turn_off_led();
        system_timer_wait_ms(500);
    }
}
