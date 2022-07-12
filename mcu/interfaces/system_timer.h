#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <stdint.h>

void system_timer_init();
void system_timer_destroy();
void system_timer_start();
void system_timer_stop();
uint32_t system_timer_get_tick_count();
void system_timer_wait_ms(uint32_t ms);
#endif
