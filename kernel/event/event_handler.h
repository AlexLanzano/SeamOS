#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/gpio.h>

#define EVENT_TYPE_NULL 0
#define EVENT_TYPE_POS_EDGE 1
#define EVENT_TYPE_NEG_EDGE 2

#define EVENT_QUEUE_LENGTH 8


typedef struct event {
    uint8_t id;
    uint8_t type;
} event_t;

typedef struct event_queue {
    uint32_t length;
    event_t events[EVENT_QUEUE_LENGTH];
} event_queue_t;

typedef struct event_source {
    bool active;
    uint8_t id;
    gpio_handle_t gpio_handle;
    bool masked;
    uint8_t input_value;
    uint32_t unmask_tick;
} event_source_t;

void event_handler_init();
void event_handler_add_event_source(uint32_t id, gpio_handle_t handle);
event_queue_t event_handler_poll();
void event_handler_clear_event_queue();
#endif
