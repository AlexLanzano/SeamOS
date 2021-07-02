#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <libraries/error.h>

typedef uint32_t gpio_handle_t;
typedef struct gpio_configuration  gpio_configuration_t;

error_t gpio_read(gpio_handle_t handle, uint8_t *value);

error_t gpio_write(gpio_handle_t handle, uint8_t value);

error_t gpio_init(gpio_configuration_t config, gpio_handle_t *handle);

#endif
