#include <kernel/task/task_manager.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <libraries/device.h>
#include <libraries/log.h>
#include <libraries/display.h>

uint32_t g_display_task_stack[500];
display_t g_display;

static void fill_screen(uint16_t color)
{
    display_draw_filled_rect(&g_display, 0, 0, g_display.width, g_display.height, color);
    display_update(&g_display, 0, 0, g_display.width, g_display.height);
}

void display_task_entry()
{
    device_open("display", &g_display.handle);
    display_init(g_display.handle, &g_display);
    log_init();

    log_info("Starting display application");

    fill_screen(0xFFFF);
    task_manager_task_wait_ms(500);
    while (1) {
        fill_screen(0xF800);
        task_manager_task_wait_ms(250);

        fill_screen(0x07E0);
        task_manager_task_wait_ms(250);

        fill_screen(0x001F);
        task_manager_task_wait_ms(250);
    }
}
