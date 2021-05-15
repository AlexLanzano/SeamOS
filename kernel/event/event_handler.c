#include <stdint.h>
#include <mcu/gpio.h>
#include <mcu/system_timer.h>
#include <kernel/event/event_handler.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>

#define EVENT_SOURCE_MAX 3
#define DEBOUNCE_THRESHOLD 5 // In millisec

static event_source_t g_source[EVENT_SOURCE_MAX];

static event_source_t *event_handler_find_inactive_event_source()
{
    event_source_t *event_source = 0;
    for (uint32_t i = 0; i < EVENT_SOURCE_MAX; i++) {
        if (g_source[i].active) {
            continue;
        }
        event_source = &g_source[i];
        break;
    }

    return event_source;
}

void event_handler_init()
{
    memset(g_source, 0, sizeof(event_source_t) * EVENT_SOURCE_MAX);
}

void event_handler_add_event_source(uint32_t id, gpio_handle_t handle)
{
    event_source_t *event_source = event_handler_find_inactive_event_source();

    if (!event_source) {
        log_error(ERROR_INVALID, "Failed to find inactive event source");
        return;
    }

    memset(event_source, 0, sizeof(event_source_t));
    event_source->active = true;
    event_source->id = id;
    event_source->gpio_handle = handle;
    gpio_read(handle, &event_source->input_value);
}


static uint8_t event_source_read_input(event_source_t *event_source)
{
    uint8_t pin_value;
    gpio_read(event_source->gpio_handle, &pin_value);
    return pin_value;
}

static void event_queue_add_event(event_queue_t *event_queue, event_t event)
{
    if (event_queue->length >= EVENT_QUEUE_LENGTH) {
        return;
    }

    event_queue->events[event_queue->length++] = event;
}

event_queue_t event_handler_poll()
{
    event_queue_t event_queue;
    event_queue.length = 0;
    for (uint32_t i = 0; i < EVENT_SOURCE_MAX; i++) {
        if (!g_source[i].active) {
            continue;
        }
        uint8_t current_input = event_source_read_input(&g_source[i]);
        if (current_input != g_source[i].input_value) {
            g_source[i].input_value = current_input;
            if (!g_source[i].masked) {
                g_source[i].masked = true;
            }
            g_source[i].unmask_tick = system_timer_get_tick_count() + DEBOUNCE_THRESHOLD;
            continue;
        }

        if (g_source[i].masked &&
            system_timer_get_tick_count() >= g_source[i].unmask_tick) {
            event_t event;
            g_source[i].masked = false;
            event.id = g_source[i].id;
            if (g_source[i].input_value) {
                event.type = EVENT_TYPE_POS_EDGE;
            } else {
                event.type = EVENT_TYPE_NEG_EDGE;
            }

            event_queue_add_event(&event_queue, event);
        }
    }
    return event_queue;
}
