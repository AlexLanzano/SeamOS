#include <stdbool.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/device/device.h>

#ifndef CONFIG_DEVICE_NAME_LENGTH
#define CONFIG_DEVICE_NAME_LENGTH 16
#endif

#ifndef CONFIG_DEVICE_MAX
#define CONFIG_DEVICE_MAX 10
#endif

#ifndef CONFIG_DEVICE_TABLE_MAX
#define CONFIG_DEVICE_TABLE_MAX 10
#endif



typedef struct device_table_entry {
    bool is_open;
    device_t *device;
} device_table_entry_t;

static device_t g_devices[CONFIG_DEVICE_MAX] = {0};
static device_table_entry_t g_device_table[CONFIG_DEVICE_TABLE_MAX] = {0};

static bool device_invalid_handle(device_handle_t handle)
{
    return (handle >= CONFIG_DEVICE_TABLE_MAX || !g_device_table[handle].is_open);
}

static error_t device_find_free_device(uint32_t *index)
{
    if (!index) {
        return ERROR_INVALID;
    }

    uint32_t device_index;

    for (device_index = 0; device_index < CONFIG_DEVICE_MAX; device_index++) {
        if (!g_devices[device_index].is_initialized) {
            *index = device_index;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static error_t device_find_free_dte(device_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    device_handle_t device_handle;

    for (device_handle = 0; device_handle < CONFIG_DEVICE_TABLE_MAX; device_handle++) {
        if (!g_device_table[device_handle].is_open) {
            *handle = device_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static error_t device_get_device_from_name(const char *name, device_t **device)
{
    if (!name || !device) {
        return ERROR_INVALID;
    }

    uint32_t device_index;
    for (device_index = 0; device_index < CONFIG_DEVICE_MAX; device_index++) {
        if (!g_devices[device_index].is_initialized) {
            continue;
        }
        if (string_is_equal_cstring(g_devices[device_index].name, name)) {
            *device = &g_devices[device_index];
            return SUCCESS;
        }
    }

    return ERROR_NOT_FOUND;
}

error_t device_init(const char *name, device_ops_t *ops, uint32_t interface_handle)
{
    if (!name) {
        return ERROR_INVALID;
    }

    error_t error;
    uint32_t index;

    error = device_find_free_device(&index);
    if (error) {
        return error;
    }

    device_t *device = &g_devices[index];
    device->name = string_init_from_cstring(device->name_data, name, CONFIG_DEVICE_NAME_LENGTH);
    device->ops = ops;
    device->interface_handle = interface_handle;
    device->is_initialized = true;

    return SUCCESS;
}

error_t device_deinit(const char *name)
{
    // TODO: Implement
    return SUCCESS;
}

error_t device_open(const char *name, device_handle_t *handle)
{
    if (!name || !handle) {
        return ERROR_INVALID;
    }

    error_t error;

    error = device_find_free_dte(handle);
    if (error) {
        return error;
    }

    device_t *device;
    device_table_entry_t *dte = &g_device_table[*handle];

    error = device_get_device_from_name(name, &device);
    if (error) {
        return ERROR_INVALID;
    }

    dte->device = device;
    dte->is_open = true;

    return SUCCESS;
}

error_t device_close(device_handle_t handle)
{
    if (device_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    device_table_entry_t *dte = &g_device_table[handle];
    dte->is_open = false;

    return SUCCESS;
}

error_t device_read(device_handle_t handle, uint8_t *data, uint32_t data_length)
{
    if (device_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    device_table_entry_t *dte = &g_device_table[handle];
    dte->device->ops->read(dte->device->interface_handle, data, data_length);

    return SUCCESS;
}

error_t device_write(device_handle_t handle, uint8_t *data, uint32_t data_length)
{
    if (device_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    device_table_entry_t *dte = &g_device_table[handle];
    dte->device->ops->write(dte->device->interface_handle, data, data_length);

    return SUCCESS;
}

error_t device_ioctl(device_handle_t handle, uint32_t cmd, void *arg)
{
    if (device_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    device_table_entry_t *dte = &g_device_table[handle];
    dte->device->ops->ioctl(dte->device->interface_handle, cmd, arg);

    return SUCCESS;
}
