#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

typedef enum display_ioctl_cmd {
    DISPLAY_IOCTL_INIT,
    DISPLAY_IOCTL_GET_INFO,
    DISPLAY_IOCTL_DIRTY_RECT
} display_ioctl_cmd_t;

typedef struct display_ioctl_get_info_args {
    uint32_t width;
    uint32_t height;
    void *framebuffer;
} display_ioctl_get_info_args_t;

typedef struct display_ioctl_dirty_rect_args {
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
} display_ioctl_dirty_rect_args_t;



#endif
