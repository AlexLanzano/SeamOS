#include <stdint.h>
#include <libraries/display.h>
#include <libraries/error.h>
#include <libraries/device.h>
#include <drivers/display/display.h>

error_t display_init(device_handle_t handle, display_t *display)
{
    display_ioctl_get_info_args_t args;
    device_ioctl(handle, DISPLAY_IOCTL_INIT, 0);
    device_ioctl(handle, DISPLAY_IOCTL_GET_INFO, &args);
    display->framebuffer = args.framebuffer;
    display->width = args.width;
    display->height = args.height;
    return SUCCESS;
}

error_t display_draw_filled_rect(display_t *display, uint32_t x, uint32_t y, uint32_t width, uint32_t height,
                                 uint16_t color)
{

    uint16_t *framebuffer = (uint16_t *)display->framebuffer + (display->width * y) + x;

    for (uint32_t i = 0; i < height; i++) {
        memset16(framebuffer, color, width);
        framebuffer += display->width;
    }

    return SUCCESS;
}

error_t display_update(display_t *display, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    display_ioctl_dirty_rect_args_t args;
    args.x = x;
    args.y = y;
    args.width = width;
    args.height = height;
    device_ioctl(display->handle, DISPLAY_IOCTL_DIRTY_RECT, &args);
    return SUCCESS;
}
