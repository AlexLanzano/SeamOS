#include <stdint.h>
#include <mcu/interfaces/gpio.h>
#include <mcu/interfaces/spi.h>
#include <mcu/interfaces/system_timer.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include <libraries/log.h>
#include <kernel/device/device.h>
#include <drivers/display/display.h>
#include <drivers/display/st7789.h>

static gpio_configuration_t g_cs_pin_config = {
    .mode = GPIO_MODE_OUTPUT,
    .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
    .pull_resistor = GPIO_PULL_RESISTOR_NONE
};

static gpio_configuration_t g_dc_pin_config = {
    .mode = GPIO_MODE_OUTPUT,
    .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
    .pull_resistor = GPIO_PULL_RESISTOR_NONE
};

static spi_configuration_t g_spi_interface_config = {
    .mode = SPI_MODE_MASTER,
    .clock_mode = SPI_CLOCK_MODE_0,
    .significant_bit = SPI_SIGNIFICANT_BIT_MSB,
    .com_mode = SPI_COM_MODE_FULL_DUPLEX,
    .data_size = SPI_DATA_SIZE_8BIT
};

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

static void st7789_spi_enable(uint32_t cs_pin)
{
    gpio_write(cs_pin, 0);
}

static void st7789_spi_disable(uint32_t cs_pin)
{
    gpio_write(cs_pin, 1);
}

static void st7789_command_mode(uint32_t dc_pin)
{
    gpio_write(dc_pin, 0);
}

static void st7789_data_mode(uint32_t dc_pin)
{
    gpio_write(dc_pin, 1);
}

static error_t st7789_send_command(const st7789_configuration_t *device,
                                   st7789_command_t command,
                                   uint8_t *data, uint32_t data_size)
{
    if (data_size && !data) {
        return ERROR_INVALID;
    }

    uint8_t command_message[1] = {command};

    st7789_spi_enable(device->cs_pin);

    st7789_command_mode(device->dc_pin);
    spi_write(device->interface_handle, &g_spi_interface_config, command_message, 1);

    st7789_data_mode(device->dc_pin);
    if (data_size) {
        spi_write(device->interface_handle, &g_spi_interface_config, data, data_size);
    }

    st7789_spi_disable(device->cs_pin);
    return SUCCESS;
}

static error_t st7789_set_frame_address(const st7789_configuration_t *device,
                                        uint16_t x0, uint16_t x1, uint16_t y0, uint16_t y1)
{
    error_t error;

    uint8_t x_bound[4] = {x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF};
    uint8_t y_bound[4] = {y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF};

    error = st7789_send_command(device, ST7789_COMMAND_CASET, x_bound, 4);
    if (error) {
        return error;
    }

    error = st7789_send_command(device, ST7789_COMMAND_RASET, y_bound, 4);
    if (error) {
        return error;
    }

    return SUCCESS;
}

error_t st7789_init(void *config)
{
    if (!config) {
        return ERROR_INVALID;
    }

    st7789_configuration_t *device_config = (st7789_configuration_t *)config;
    uint32_t cs_pin = device_config->cs_pin;
    uint32_t dc_pin = device_config->dc_pin;

    error_t error;
    error = gpio_init(cs_pin, &g_cs_pin_config);
    if (error) {
        return error;
    }

    error = gpio_init(dc_pin, &g_dc_pin_config);
    if (error) {
        return error;
    }

    return SUCCESS;
}

static error_t st7789_display_init(st7789_configuration_t *device_config)
{
    // TODO: Please implement a timer that doesnt require to be in
    // task context so this can sit in regular init function

    error_t error;

    // Reset Display
    error = st7789_send_command(device_config, ST7789_COMMAND_SWRESET, 0, 0);
    if (error) {
        return error;
    }
    system_timer_wait_ms(150);

    // Wake up from sleep
    error = st7789_send_command(device_config, ST7789_COMMAND_SLPOUT, 0, 0);
    if (error) {
        return error;
    }
    system_timer_wait_ms(10);

    // Set color format to 16bit
    error = st7789_send_command(device_config, ST7789_COMMAND_COLMOD, &device_config->color_format, 1);
    if (error) {
        return error;
    }

    // Set memory access to "Top to bottom"
    uint8_t arg = 0;
    error = st7789_send_command(device_config, ST7789_COMMAND_MADCTL, &arg, 1);
    if (error) {
        return error;
    }

    error = st7789_set_frame_address(device_config, 0, 0, device_config->width-1, device_config->height-1);
    if (error) {
        return error;
    }

    // Inverse display
    error = st7789_send_command(device_config, ST7789_COMMAND_INVON, 0, 0);
    if (error) {
        return error;
    }

    // Turn on normal display mode
    error = st7789_send_command(device_config, ST7789_COMMAND_NORON, 0, 0);
    if (error) {
        return error;
    }
    system_timer_wait_ms(10);

    // Turn on display
    error = st7789_send_command(device_config, ST7789_COMMAND_DISPON, 0, 0);
    if (error) {
        return error;
    }

    system_timer_wait_ms(100);
    return SUCCESS;
}

error_t st7789_deinit(void *config)
{
    // TODO: Implement
    return ERROR_NOT_IMPLEMENTED;
}

error_t st7789_read(void *config, void *data, uint32_t data_length)
{
    return ERROR_NOT_IMPLEMENTED;
}

error_t st7789_write(void *config, void *data, uint32_t data_length)
{
    return ERROR_NOT_IMPLEMENTED;
}

static error_t st7789_update_dirty_rect(const st7789_configuration_t *device,
                                        uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    uint8_t *data = (uint8_t *)((uint32_t)device->framebuffer + (y*width) + x);
    uint8_t command_message[1] = {ST7789_COMMAND_RAMWR};
    st7789_set_frame_address(device, x, x+width-1, y, y+height-1);

    st7789_spi_enable(device->cs_pin);

    st7789_command_mode(device->dc_pin);
    spi_write(device->interface_handle, &g_spi_interface_config, command_message, 1);

    system_timer_stop();

    st7789_data_mode(device->dc_pin);
    do {
        spi_write_dma(device->interface_handle, &g_spi_interface_config, data, sizeof(uint16_t) * width);
        data += sizeof(uint16_t) * width;
    } while (height--);
    st7789_spi_disable(device->cs_pin);

    system_timer_start();

    return SUCCESS;
}

error_t st7789_ioctl(void *config, uint32_t cmd, void *arg)
{
    if (!config) {
        return ERROR_INVALID;
    }

    st7789_configuration_t *device_config = (st7789_configuration_t *)config;

    switch (cmd) {
    case DISPLAY_IOCTL_INIT:
    {
        // TODO: Please implement a timer that doesnt require to be in
        // task context so this can sit in regular init function
        st7789_display_init(device_config);
        break;
    }
    case DISPLAY_IOCTL_GET_INFO:
    {
        display_ioctl_get_info_args_t *args = (display_ioctl_get_info_args_t *)arg;
        args->width = device_config->width;
        args->height = device_config->height;
        args->framebuffer = device_config->framebuffer;
        break;
    }
    case DISPLAY_IOCTL_DIRTY_RECT:
    {
        display_ioctl_dirty_rect_args_t *args = (display_ioctl_dirty_rect_args_t *)arg;
        st7789_update_dirty_rect(device_config, args->x, args->y, args->width, args->height);
        break;
    }
    default:
        return ERROR_INVALID;
    }

    return SUCCESS;
}

device_ops_t g_st7789_device_ops = {
    .init = st7789_init,
    .deinit = st7789_deinit,
    .read = st7789_read,
    .write = st7789_write,
    .ioctl = st7789_ioctl
};
