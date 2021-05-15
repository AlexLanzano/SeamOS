#ifndef BLINK_H
#define BLINK_H

typedef struct blink_application_data {
    gpio_handle_t gpio_handle;
} blink_application_data_t;

void blink_application_start();

#endif
