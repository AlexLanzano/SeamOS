#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/device/device.h>
#include <mcu/interfaces/gpio.h>

gpio_interface_configuration_t g_led_device_config =
    (gpio_interface_configuration_t)
    {
     .mode = GPIO_MODE_OUTPUT,
     .pull_resistor = GPIO_PULL_RESISTOR_UP
    };

error_t led_init(uint32_t interface_handle)
{   
    return gpio_init(interface_handle, &g_led_device_config);
}

error_t led_deinit(uint32_t interface_handle)
{
    return gpio_deinit(interface_handle);
}

error_t led_read(uint32_t interface_handle, uint8_t *data, uint32_t data_length)
{
    return gpio_read(interface_handle, data);
}

error_t led_write(uint32_t interface_handle, uint8_t *data, uint32_t data_length)
{
    return gpio_write(interface_handle, *data);
}

error_t led_ioctl(uint32_t interface_handle, uint32_t cmd, void *arg)
{
    return ERROR_NOT_IMPLEMENTED;
}

device_ops_t g_led_device_ops =
    (device_ops_t)
    {
     .init = led_init,
     .deinit = led_deinit,
     .read = led_read,
     .write = led_write,
     .ioctl = led_ioctl
    };
