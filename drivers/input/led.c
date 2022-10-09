#include <drivers/input/led.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/device/device.h>
#include <mcu/interfaces/gpio.h>

gpio_configuration_t g_led_device_config = {
    .mode = GPIO_MODE_OUTPUT,
    .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
    .pull_resistor = GPIO_PULL_RESISTOR_UP
};

error_t led_init(void *config)
{
    if (!config) {
        return ERROR_INVALID;
    }

    led_configuration_t *led_config = (led_configuration_t *)config;
    return gpio_init(led_config->pin, &g_led_device_config);
}

error_t led_deinit(void *config)
{
    if (!config) {
        return ERROR_INVALID;
    }

    led_configuration_t *led_config = (led_configuration_t *)config;
    return gpio_deinit(led_config->pin);
}

error_t led_read(void *config, void *data, uint32_t data_length)
{
    if (!config || !data) {
        return ERROR_INVALID;
    }

    led_configuration_t *led_config = (led_configuration_t *)config;
    return gpio_read(led_config->pin, (uint8_t *)data);
}

error_t led_write(void *config, void *data, uint32_t data_length)
{
    if (!config || !data) {
        return ERROR_INVALID;
    }

    led_configuration_t *led_config = (led_configuration_t *)config;
    return gpio_write(led_config->pin, *(uint8_t *)data);
}

error_t led_ioctl(void *config, uint32_t cmd, void *arg)
{
    return ERROR_NOT_IMPLEMENTED;
}

device_ops_t g_led_device_ops = {
    .init = led_init,
    .deinit = led_deinit,
    .read = led_read,
    .write = led_write,
    .ioctl = led_ioctl
};
