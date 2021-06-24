#include <stm32wb55xx/gpio.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>
#include <applications/blink.h>

uint32_t g_blink_stack[500];
static gpio_handle_t g_gpio_handle;

static inline void turn_on_led()
{
    gpio_write(g_gpio_handle, 1);
}

static inline void turn_off_led()
{
    gpio_write(g_gpio_handle, 0);
}

void blink_task_entry()
{
    log_info("Starting blink application");
    gpio_configure_pin((gpio_configuration_t)
                       {.port = GPIOA,
                        .pin = 0,
                        .mode = GPIO_MODE_OUTPUT,
                        .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                        .output_speed = GPIO_OUTPUT_SPEED_LOW,
                        .pull_resistor = GPIO_PULL_RESISTOR_NONE},
                        &g_gpio_handle);

    while (1) {
        log_info("Blink!");
        turn_on_led();
        task_manager_task_wait_ms(500);
        turn_off_led();
        task_manager_task_wait_ms(500);
    }
}
