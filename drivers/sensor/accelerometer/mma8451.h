#ifndef MMA8451_H
#define MMA8451_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/i2c.h>
#include <libraries/error.h>

typedef struct mma8451_configuration {
    bool is_open;
    i2c_handle_t i2c_handle;
} mma8451_configuration_t;

typedef uint32_t mma8451_handle_t;

error_t mma8451_init(mma8451_configuration_t config, mma8451_handle_t *handle);
error_t mma8451_deinit(mma8451_handle_t handle);
error_t mma8451_read(mma8451_handle_t handle, uint32_t *x, uint32_t *y, uint32_t *z);

#endif
