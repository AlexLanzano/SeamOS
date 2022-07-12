#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/device.h>
#include <libraries/log.h>
#include <applications/blink.h>

uint32_t g_blink_stack[500];
static device_handle_t g_led_handle;

static inline void turn_on_led()
{
    uint8_t off = 1;
    device_write(g_led_handle, &off, 1);
}

static inline void turn_off_led()
{
    uint8_t on = 0;
    device_write(g_led_handle, &on, 1);
}

void blink_task_entry()
{
    device_open("led", &g_led_handle);
    log_init();

    log_info("Starting blink application");

    while (1) {
        log_info("Blink!");
        turn_on_led();
        task_manager_task_wait_ms(500);
        turn_off_led();
        task_manager_task_wait_ms(500);
    }
}
