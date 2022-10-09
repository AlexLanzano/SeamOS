#ifndef MMA8451_H
#define MMA8451_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/interfaces/i2c.h>
#include <libraries/error.h>

typedef struct mma8451_configuration {
    uint32_t interface_handle;
} mma8451_configuration_t;

typedef uint32_t mma8451_handle_t;

error_t mma8451_init(void *config);
error_t mma8451_deinit(void *config);
error_t mma8451_read(void *config, void *buffer, uint32_t count);

#endif
