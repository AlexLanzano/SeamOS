#include <mcu/i2c.h>
#include <drivers/input/ft6336g.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>
#include <kernel/task/task_manager.h>
#include <arch/arch.h>

#define FT6336G_DEVICE_MAX 1
typedef enum ft6336g_register {
    FT6336G_REGISTER_DEV_MODE,
    FT6336G_REGISTER_GEST_ID,
    FT6336G_REGISTER_TD_STATUS,
    FT6336G_REGISTER_P1_XH,
    FT6336G_REGISTER_P1_XL,
    FT6336G_REGISTER_P1_YH,
    FT6336G_REGISTER_P1_YL,
    FT6336G_REGISTER_P1_WEIGHT,
    FT6336G_REGISTER_P1_MISC,
    FT6336G_REGISTER_P2_XH,
    FT6336G_REGISTER_P2_XL,
    FT6336G_REGISTER_P2_YH,
    FT6336G_REGISTER_P2_YL,
    FT6336G_REGISTER_P2_WEIGHT,
    FT6336G_REGISTER_P2_MISC,
    FT6336G_REGISTER_TH_GROUP = 0x80,
    FT6336G_REGISTER_TH_DIFF = 0x85,
    FT6336G_REGISTER_CTRL,
    FT6336G_REGISTER_TIMEENTERMONITOR,
    FT6336G_REGISTER_PERIODACTIVE,
    FT6336G_REGISTER_PERIODMONITOR,
    FT6336G_REGISTER_RADIAN_VALUE = 0x91,
    FT6336G_REGISTER_OFFSET_LEFT_RIGHT,
    FT6336G_REGISTER_OFFSET_UP_DOWN,
    FT6336G_REGISTER_DISTANCE_LEFT_RIGHT,
    FT6336G_REGISTER_DISTANCE_UP_DOWN,
    FT6336G_REGISTER_DISTANCE_ZOOM,
    FT6336G_REGISTER_LIB_VER_H = 0xA1,
    FT6336G_REGISTER_LIB_VER_L,
    FT6336G_REGISTER_CIPHER,
    FT6336G_REGISTER_G_MODE,
    FT6336G_REGISTER_PWR_MODE,
    FT6336G_REGISTER_FIRMID,
    FT6336G_REGISTER_FOCALTECH_ID,
    FT6336G_REGISTER_RELEASE_CODE_ID = 0xAF,
    FT6336G_REGISTER_STATE = 0xBC
} ft6336g_register_t;

typedef struct ft6336g_device {
    bool is_initialized;
    ft6336g_configuration_t config;
} ft6336g_device_t;

static ft6336g_device_t g_ft6336g_devices[FT6336G_DEVICE_MAX];

static bool ft6336g_invalid_handle(ft6336g_handle_t handle)
{
    return (handle >= FT6336G_DEVICE_MAX || !g_ft6336g_devices[handle].is_initialized);
}

static error_t ft6336g_find_free_device(ft6336g_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    for (uint32_t i = 0; i < FT6336G_DEVICE_MAX; i++) {
        if (!g_ft6336g_devices[i].is_initialized) {
            *handle = i;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

/* static error_t ft6336g_read_register(ft6336g_handle_t handle, ft6336g_register_t reg, uint8_t *data) */
/* { */
/*     if (ft6336g_invalid_handle(handle) || !data) { */
/*         return ERROR_INVALID; */
/*     } */

/*     ft6336g_configuration_t *config = &g_ft6336g_devices[handle].config; */
/*     uint8_t address = reg; */

/*     // Set the register to read from */
/*     i2c_write_byte(config->i2c_handle, address); */
/*     i2c_stop(config->i2c_handle); */

/*     // Read the register */
/*     i2c_read_byte(config->i2c_handle, data); */
/*     i2c_stop(config->i2c_handle); */

/*     return SUCCESS; */
/* } */

static error_t ft6336g_read_registers(ft6336g_handle_t handle, ft6336g_register_t reg, uint8_t *data,
                                      uint32_t data_size)
{
    if (ft6336g_invalid_handle(handle) || !data) {
        return ERROR_INVALID;
    }

    ft6336g_configuration_t *config = &g_ft6336g_devices[handle].config;
    uint8_t address = reg;

    arch_disable_irq();

    // Set the register to read from
    i2c_write_byte(config->i2c_handle, address);
    task_manager_task_wait_ms(1);
    i2c_stop(config->i2c_handle);

    // Read the registers
    i2c_read(config->i2c_handle, data, data_size);
    i2c_stop(config->i2c_handle);

    arch_enable_irq();

    return SUCCESS;
}

static error_t ft6336g_write_register(ft6336g_handle_t handle, ft6336g_register_t reg, uint8_t data)
{
    if (ft6336g_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    ft6336g_configuration_t *config = &g_ft6336g_devices[handle].config;
    uint8_t address = reg;
    uint8_t msg[2] = {address, data};

    i2c_write(config->i2c_handle, msg, 2);
    i2c_stop(config->i2c_handle);

    return SUCCESS;
}

error_t ft6336g_init(ft6336g_configuration_t config, ft6336g_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;
    ft6336g_device_t *device;

    error = ft6336g_find_free_device(handle);
    if (error) {
        return error;
    }

    device = &g_ft6336g_devices[*handle];
    device->is_initialized = true;
    memcpy(&device->config, &config, sizeof(ft6336g_configuration_t));

    // Set threshold for touch detection
    ft6336g_write_register(*handle, FT6336G_REGISTER_TH_GROUP, device->config.threshold);

    // Set interrupt mode
    ft6336g_write_register(*handle, FT6336G_REGISTER_G_MODE, device->config.interrupt_mode);

    return SUCCESS;
}

error_t ft6336g_deinit(ft6336g_handle_t handle)
{
    // TODO: Implement
    return SUCCESS;
}

error_t ft6336g_read(ft6336g_handle_t handle, uint16_t *x, uint16_t *y, ft6336g_event_flag_t *event_flag)
{
    uint8_t data[6];

    ft6336g_read_registers(handle, FT6336G_REGISTER_P1_XH, data, 6);

    *x = ((data[1] & 0xf) << 8) | data[2];
    *y = (data[3] << 8) | data[4];
    *event_flag = data[1] >> 6;

    return SUCCESS;
}
