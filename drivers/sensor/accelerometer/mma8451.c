#include <drivers/sensor/accelerometer/mma8451.h>
#include <kernel/device/device.h>
#include <mcu/interfaces/gpio.h>
#include <libraries/error.h>
#include <libraries/string.h>

#define MMA8451_DEVICE_ADDRESS 0x1d
#define MMA8451_REG_OUT_X_MSB (0x01)

static i2c_configuration_t g_i2c_interface_config = {
    .address = 0x1d,
    .address_mode = I2C_ADDRESS_MODE_7BIT
};

error_t mma8451_init(void *config)
{
    mma8451_configuration_t *device_config = (mma8451_configuration_t *)config;

    // Set active mode and low noise mode
    uint8_t data[2];
    data[0] = 0x2a;
    data[1] = 0x01 | 0x4;
    i2c_write(device_config->interface_handle, &g_i2c_interface_config, data, 2);
    i2c_stop(device_config->interface_handle);

    return SUCCESS;
}

error_t mma8451_deinit(void *config)
{
    return SUCCESS;
}

error_t mma8451_read(void *config, void *buffer, uint32_t count)
{
    (void)count;

    mma8451_configuration_t *device = (mma8451_configuration_t *)config;

    uint32_t value = 0;
    uint32_t *x = &((uint32_t *)buffer)[0];
    uint32_t *y = &((uint32_t *)buffer)[1];
    uint32_t *z = &((uint32_t *)buffer)[2];

    *x = 0;
    *y = 0;
    *z = 0;

    i2c_write_byte(device->interface_handle, &g_i2c_interface_config, 0x1);
    i2c_read_byte(device->interface_handle, &g_i2c_interface_config, (uint8_t *)&value);
    i2c_stop(device->interface_handle);
    *x |= value << 8;
    value = 0;

    i2c_write_byte(device->interface_handle, &g_i2c_interface_config, 0x2);
    i2c_read_byte(device->interface_handle, &g_i2c_interface_config, (uint8_t *)&value);
    i2c_stop(device->interface_handle);
    *x |= value >> 2;
    value = 0;

    i2c_write_byte(device->interface_handle, &g_i2c_interface_config, 0x3);
    i2c_read_byte(device->interface_handle, &g_i2c_interface_config, (uint8_t *)&value);
    i2c_stop(device->interface_handle);
    *y |= value << 8;
    value = 0;

    i2c_write_byte(device->interface_handle, &g_i2c_interface_config, 0x4);
    i2c_read_byte(device->interface_handle, &g_i2c_interface_config, (uint8_t *)&value);
    i2c_stop(device->interface_handle);
    *y |= value >> 2;
    value = 0;

    i2c_write_byte(device->interface_handle, &g_i2c_interface_config, 0x5);
    i2c_read_byte(device->interface_handle, &g_i2c_interface_config, (uint8_t *)&value);
    i2c_stop(device->interface_handle);
    *z |= value << 8;
    value = 0;

    i2c_write_byte(device->interface_handle, &g_i2c_interface_config, 0x6);
    i2c_read_byte(device->interface_handle, &g_i2c_interface_config, (uint8_t *)&value);
    i2c_stop(device->interface_handle);
    *z |= value >> 2;
    value = 0;

    return SUCCESS;
}

error_t mma8451_write(void *config, void *buffer, uint32_t count)
{
    return ERROR_NOT_IMPLEMENTED;
}

error_t mma8451_ioctl(void *config, uint32_t cmd, void *arg)
{
    return ERROR_NOT_IMPLEMENTED;
}

device_ops_t g_mma8451_device_ops = {
    .init = mma8451_init,
    .deinit = mma8451_deinit,
    .read = mma8451_read,
    .write = mma8451_write,
    .ioctl = mma8451_ioctl
};
