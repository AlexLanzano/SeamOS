#ifndef BLUEFRUIT_H
#define BLUEFRUIT_H

#include <stdint.h>
#include <mcu/gpio.h>
#include <mcu/spi.h>
#include <libraries/error.h>

typedef struct bluefruit_configuration {
    gpio_handle_t irq_pin_handle;
    spi_device_handle_t spi_handle;
} bluefruit_configuration_t;

error_t bluefruit_read(void *buffer, uint32_t size);
error_t bluefruit_write(void *buffer, uint32_t size);
error_t bluefruit_init(bluefruit_configuration_t config);

#endif
