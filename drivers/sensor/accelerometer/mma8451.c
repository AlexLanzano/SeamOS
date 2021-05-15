#include <drivers/sensor/accelerometer/mma8451.h>
#include <libraries/error.h>
#include <libraries/string.h>

#define MMA8451_DEVICE_ADDRESS 0x1d
#define MMA8451_REG_OUT_X_MSB (0x01)

mma8451_configuration_t g_configuration;
mma8451_handle_t g_handle = 0;

error_t mma8451_init(mma8451_configuration_t config, i2c_handle_t *handle)
{

    g_configuration = config;

    // Set active mode and low noise mode
    uint8_t data[2];
    data[0] = 0x2a;
    data[1] = 0x01 | 0x4;
    i2c_write(config.i2c_handle, data, 2);
    i2c_stop(*handle);

    return SUCCESS;
}

error_t mma8451_deinit(uint32_t handle)
{
    return SUCCESS;
}

error_t mma8451_read(uint32_t handle, uint32_t *x, uint32_t *y, uint32_t *z)
{
    (void)handle;
    uint32_t value = 0;
    *x = 0;
    *y = 0;
    *z = 0;

    i2c_write_byte(g_configuration.i2c_handle, 0x1);
    i2c_read_byte(g_configuration.i2c_handle, (uint8_t *)&value);
    i2c_stop(g_configuration.i2c_handle);
    *x |= value << 8;
    value = 0;

    i2c_write_byte(g_configuration.i2c_handle, 0x2);
    i2c_read_byte(g_configuration.i2c_handle, (uint8_t *)&value);
    i2c_stop(g_configuration.i2c_handle);
    *x |= value >> 2;
    value = 0;

    i2c_write_byte(g_configuration.i2c_handle, 0x3);
    i2c_read_byte(g_configuration.i2c_handle, (uint8_t *)&value);
    i2c_stop(g_configuration.i2c_handle);
    *y |= value << 8;
    value = 0;

    i2c_write_byte(g_configuration.i2c_handle, 0x4);
    i2c_read_byte(g_configuration.i2c_handle, (uint8_t *)&value);
    i2c_stop(g_configuration.i2c_handle);
    *y |= value >> 2;
    value = 0;

    i2c_write_byte(g_configuration.i2c_handle, 0x5);
    i2c_read_byte(g_configuration.i2c_handle, (uint8_t *)&value);
    i2c_stop(g_configuration.i2c_handle);
    *z |= value << 8;
    value = 0;

    i2c_write_byte(g_configuration.i2c_handle, 0x6);
    i2c_read_byte(g_configuration.i2c_handle, (uint8_t *)&value);
    i2c_stop(g_configuration.i2c_handle);
    *z |= value >> 2;
    value = 0;

    return SUCCESS;
}
