#ifndef SYSTICK_TIMER_H
#define SYSTICK_TIMER_H

void systick_timer_init();
void systick_timer_destroy();
void systick_timer_start();
void systick_timer_stop();
uint32_t systick_timer_get_tick_count();
void systick_timer_wait_ms(uint32_t ms);
#endif
