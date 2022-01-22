#include <stdint.h>
#include <mcu/gpio.h>
#include <mcu/spi.h>
#include <mcu/system_timer.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <kernel/debug/log.h>
#include <drivers/display/st7789.h>

#define ST7789_DEVICE_MAX 1

#define ST7789_COMMAND_MODE(handle) gpio_write(g_st7789_devices[handle].config.dc_pin_handle, 0)
#define ST7789_DATA_MODE(handle) gpio_write(g_st7789_devices[handle].config.dc_pin_handle, 1)

typedef struct st7789_device {
    bool is_initialized;
    st7789_configuration_t config;
} st7789_device_t;

typedef enum {
    ST7789_COMMAND_NOP = 0x00,
    ST7789_COMMAND_SWRESET = 0x01,
    ST7789_COMMAND_RDDID = 0x04,
    ST7789_COMMAND_RDDST = 0x09,
    ST7789_COMMAND_RDDPM = 0x0a,
    ST7789_COMMAND_RDDMADCTL = 0x0b,
    ST7789_COMMAND_RDDCOLMOD = 0x0c,
    ST7789_COMMAND_RDDIM = 0x0d,
    ST7789_COMMAND_RDDSM = 0x0e,
    ST7789_COMMAND_RDDSDR = 0x0f,
    ST7789_COMMAND_SLPIN = 0x10,
    ST7789_COMMAND_SLPOUT = 0x11,
    ST7789_COMMAND_PTLON = 0x12,
    ST7789_COMMAND_NORON = 0x13,
    ST7789_COMMAND_INVOFF = 0x20,
    ST7789_COMMAND_INVON = 0x21,
    ST7789_COMMAND_GAMSET = 0x26,
    ST7789_COMMAND_DISPOFF = 0x28,
    ST7789_COMMAND_DISPON = 0x29,
    ST7789_COMMAND_CASET = 0x2a,
    ST7789_COMMAND_RASET = 0x2b,
    ST7789_COMMAND_RAMWR = 0x2c,
    ST7789_COMMAND_RAMRD = 0x2e,
    ST7789_COMMAND_PTLAR = 0x30,
    ST7789_COMMAND_VSCRDEF = 0x33,
    ST7789_COMMAND_TEOFF = 0x34,
    ST7789_COMMAND_TEON = 0x35,
    ST7789_COMMAND_MADCTL = 0x36,
    ST7789_COMMAND_VSCSAD = 0x37,
    ST7789_COMMAND_IDMOFF = 0x38,
    ST7789_COMMAND_IDMON = 0x39,
    ST7789_COMMAND_COLMOD = 0x3a,
    ST7789_COMMAND_WRMEMC = 0x3c,
    ST7789_COMMAND_RDMEMC = 0x3e,
    ST7789_COMMAND_STE = 0x44,
    ST7789_COMMAND_GSCAN = 0x45,
    ST7789_COMMAND_WRDISBV = 0x51,
    ST7789_COMMAND_RDDISBV = 0x52,
    ST7789_COMMAND_WRCTRLD = 0x53,
    ST7789_COMMAND_RDCTRLD = 0x54,
    ST7789_COMMAND_WRCACE = 0x55,
    ST7789_COMMAND_RDCABC = 0x56,
    ST7789_COMMAND_WRCABCMB = 0x5e,
    ST7789_COMMAND_RDCABCMB = 0x5f,
    ST7789_COMMAND_RDABCSDR = 0x68,
    ST7789_COMMAND_RDID1 = 0xda,
    ST7789_COMMAND_RDID2 = 0xdb,
    ST7789_COMMAND_RDID3 = 0xdc,
    ST7789_COMMAND_RAMCTRL = 0xb0,
    ST7789_COMMAND_RGBCTRL = 0xb1,
    ST7789_COMMAND_PORCTRL  = 0xb2,
    ST7789_COMMAND_FRCTRL1 = 0xb3,
    ST7789_COMMAND_PARCTRL = 0xb5,
    ST7789_COMMAND_GCTRL = 0xb7,
    ST7789_COMMAND_GTADJ = 0xb8,
    ST7789_COMMAND_DGMEN = 0xba,
    ST7789_COMMAND_VCOMS = 0xbb,
    ST7789_COMMAND_LCMCTRL = 0xc0,
    ST7789_COMMAND_IDSET = 0xc1,
    ST7789_COMMAND_VDVVRHEN = 0xc2,
    ST7789_COMMAND_VRHS = 0xc3,
    ST7789_COMMAND_VDVS = 0xc4,
    ST7789_COMMAND_VCMOFSET = 0xc5,
    ST7789_COMMAND_FRCTRL2 = 0xc6,
    ST7789_COMMAND_CABCCTRL = 0xc7,
    ST7789_COMMAND_REGSEL1 = 0xc8,
    ST7789_COMMAND_REGSEL2 = 0xca,
    ST7789_COMMAND_PWMFRSEL = 0xcc,
    ST7789_COMMAND_PWCTRL1 = 0xd0,
    ST7789_COMMAND_VAPVANEN = 0xd2,
    ST7789_COMMAND_CMD2EN = 0xdf,
    ST7789_COMMAND_PVGAMCTRL = 0xe0,
    ST7789_COMMAND_NVGAMCTRL = 0xe1,
    ST7789_COMMAND_DGMLUTR = 0xe2,
    ST7789_COMMAND_DGMLUTB = 0xe3,
    ST7789_COMMAND_GATECTRL = 0xe4,
    ST7789_COMMAND_SPI2EN = 0xe7,
    ST7789_COMMAND_PWCTRL2 = 0xe8,
    ST7789_COMMAND_EQCTRL = 0xe9,
    ST7789_COMMAND_PROMCTRL = 0xec,
    ST7789_COMMAND_PROMEN = 0xfa,
    ST7789_COMMAND_NVMSET = 0xfc,
    ST7789_COMMAND_PROMACT = 0xfe,
} st7789_command_t;

static st7789_device_t g_st7789_devices[ST7789_DEVICE_MAX];

static bool st7789_invalid_handle(st7789_handle_t handle)
{
    return (handle >= ST7789_DEVICE_MAX || !g_st7789_devices[handle].is_initialized);
}

static error_t st7789_find_free_device(st7789_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    for (uint32_t i = 0; i < ST7789_DEVICE_MAX; i++) {
        if (!g_st7789_devices[i].is_initialized) {
            *handle = i;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static error_t st7789_send_command(st7789_handle_t handle, st7789_command_t command,
                                   uint8_t *data, uint32_t data_size)
{
    if (st7789_invalid_handle(handle) ||
        (data_size && !data)) {
        return ERROR_INVALID;
    }

    st7789_configuration_t *display = &g_st7789_devices[handle].config;
    uint8_t command_message[1] = {command};

    spi_device_enable(display->spi_handle);

    ST7789_COMMAND_MODE(handle);

    spi_write(display->spi_handle, command_message, 1);
    ST7789_DATA_MODE(handle);
    if (data_size) {
        spi_write(display->spi_handle, data, data_size);
    }

    spi_device_disable(display->spi_handle);

    return SUCCESS;
}
static error_t st7789_send_command_repeat(st7789_handle_t handle, st7789_command_t command,
                                          uint8_t *data, uint32_t data_size, uint32_t data_count)
{
    if (st7789_invalid_handle(handle) ||
        (data_size && !data)) {
        return ERROR_INVALID;
    }

    st7789_configuration_t *display = &g_st7789_devices[handle].config;
    uint8_t command_message[1] = {command};

    spi_device_enable(display->spi_handle);

    ST7789_COMMAND_MODE(handle);
    spi_write(display->spi_handle, command_message, 1);

    system_timer_stop();
    ST7789_DATA_MODE(handle);
    while (data_count--) {
        spi_write_dma(display->spi_handle, data, data_size);
    }
    spi_device_disable(display->spi_handle);
    system_timer_start();
    return SUCCESS;
}

static error_t st7789_render_framebuffer(st7789_handle_t handle, uint8_t *data, uint32_t width, uint32_t height)
{
    if (st7789_invalid_handle(handle) ||
        (!data)) {
        return ERROR_INVALID;
    }

    st7789_configuration_t *display = &g_st7789_devices[handle].config;
    uint8_t command_message[1] = {ST7789_COMMAND_RAMWR};

    spi_device_enable(display->spi_handle);

    ST7789_COMMAND_MODE(handle);
    spi_write(display->spi_handle, command_message, 1);

    system_timer_stop();
    ST7789_DATA_MODE(handle);
    while (height--) {
        spi_write_dma(display->spi_handle, data, sizeof(color16_t) * width);
        data += sizeof(color16_t) * display->width;
    }
    spi_device_disable(display->spi_handle);
    system_timer_start();
    return SUCCESS;
}

static error_t st7789_get_range(uint16_t start, uint16_t end, uint8_t *range)
{
    if (!range) {
        return ERROR_INVALID;
    }

    range[0] = (start >> 8) & 0xff;
    range[1] = start & 0xff;
    range[2] = (end >> 8) & 0xff;
    range[3] = end & 0xff;

    return SUCCESS;
}

error_t st7789_init(st7789_configuration_t config, st7789_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;
    st7789_device_t *device;
    uint8_t arg;

    error = st7789_find_free_device(handle);
    if (error) {
        log_error(error, "Failed to find free st7789 device");
        return error;
    }

    device = &g_st7789_devices[*handle];
    device->is_initialized = true;
    memcpy(&device->config, &config, sizeof(st7789_configuration_t));

    // Reset Display
    error = st7789_send_command(*handle, ST7789_COMMAND_SWRESET, 0, 0);
    if (error) {
        log_error(error, "Failed to reset st7789 display");
        return error;
    }
    system_timer_wait_ms(150);

    // Wake up from sleep
    error = st7789_send_command(*handle, ST7789_COMMAND_SLPOUT, 0, 0);
    if (error) {
        log_error(error, "Failed to wake display up from sleep");
        return error;
    }
    system_timer_wait_ms(10);

    // Set color format to 16bit
    error = st7789_send_command(*handle, ST7789_COMMAND_COLMOD, &config.color_format, 1);
    if (error) {
        log_error(error, "Failed to set color format");
        return error;
    }

    // Set memory access to "Top to bottom"
    arg = 0;
    error = st7789_send_command(*handle, ST7789_COMMAND_MADCTL, &arg, 1);
    if (error) {
        log_error(error, "Failed to set memory access");
        return error;
    }

    // Set column range
    uint8_t range[4];
    st7789_get_range(0, config.width, range);
    error = st7789_send_command(*handle, ST7789_COMMAND_CASET, range, 4);
    if (error) {
        log_error(error, "Failed to set column range");
        return error;
    }

    // Set row range
    st7789_get_range(0, config.height, range);
    error = st7789_send_command(*handle, ST7789_COMMAND_RASET, range, 4);
    if (error) {
        log_error(error, "Failed to set row range");
        return error;
    }

    // Inverse display
    error = st7789_send_command(*handle, ST7789_COMMAND_INVON, 0, 0);
    if (error) {
        log_error(error, "Failed to inverse display");
        return error;
    }

    // Turn on normal display mode
    error = st7789_send_command(*handle, ST7789_COMMAND_NORON, 0, 0);
    if (error) {
        log_error(error, "Failed to set display to normal mode");
        return error;
    }
    system_timer_wait_ms(10);

    // Turn on display
    error = st7789_send_command(*handle, ST7789_COMMAND_DISPON, 0, 0);
    if (error) {
        log_error(error, "Failed to turn on display");
        return error;
    }
    system_timer_wait_ms(100);

    return SUCCESS;
}

error_t st7789_deinit(st7789_handle_t handle)
{
    if (st7789_invalid_handle(handle)) {
        return ERROR_INVALID;
    }
    // TODO: implement
    return SUCCESS;
}

error_t st7789_draw_pixel(st7789_handle_t handle, uint32_t x, uint32_t y, color16_t color)
{
    if (st7789_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    uint8_t range[4];

    st7789_get_range(x, x, range);
    st7789_send_command(handle, ST7789_COMMAND_CASET, range, 4);

    st7789_get_range(y, y, range);
    st7789_send_command(handle, ST7789_COMMAND_RASET, range, 4);

    st7789_send_command(handle, ST7789_COMMAND_RAMWR, (uint8_t *)&color, 2);

    return SUCCESS;
}

error_t st7789_draw_filled_rect(st7789_handle_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, color16_t color)
{
    if (st7789_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    uint8_t range[4];

    st7789_get_range(0, width, range);
    st7789_send_command(handle, ST7789_COMMAND_CASET, range, 4);

    st7789_get_range(0, height, range);
    st7789_send_command(handle, ST7789_COMMAND_RASET, range, 4);

    color16_t buffer[width];
    for (uint32_t i = 0; i < width; i++) {
        buffer[i] = color;
    }
    st7789_send_command_repeat(handle, ST7789_COMMAND_RAMWR, (uint8_t *)buffer,
                               sizeof(color16_t) * width, height);
    return SUCCESS;
}

error_t st7789_draw_rect(st7789_handle_t handle, uint32_t x, uint32_t y, uint32_t width, uint32_t height, color16_t *data)
{
    if (st7789_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    uint8_t range[4];

    st7789_get_range(x, width+x, range);
    st7789_send_command(handle, ST7789_COMMAND_CASET, range, 4);

    st7789_get_range(y, height+y, range);
    st7789_send_command(handle, ST7789_COMMAND_RASET, range, 4);

    st7789_render_framebuffer(handle, (uint8_t *)data, width, height);

    return SUCCESS;
}

error_t st7789_clear(st7789_handle_t handle)
{
    if (st7789_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    st7789_configuration_t *config = &g_st7789_devices[handle].config;
    return st7789_draw_filled_rect(handle, 0, 0, config->width, config->height, 0xffff);
}
