#include <config.h>
#include <arch/arch.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/interfaces/system_timer.h>
#include <kernel/task/task_manager.h>

uint32_t tick_count = 0;

void system_timer_init()
{
    SysTick->LOAD = CONFIG_CLOCK_FREQ / 1000;
    SysTick->VAL = 0;
    SysTick->CTRL = (SysTick_CTRL_CLKSOURCE_Msk |
                     SysTick_CTRL_TICKINT_Msk);
}

void system_timer_destroy()
{
    // TODO
    return;
}

void system_timer_reset()
{
    tick_count = 0;
}

inline void system_timer_start()
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

inline void system_timer_stop()
{
    SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk);
}

uint32_t system_timer_get_tick_count()
{
    return tick_count;
}

void system_timer_wait_ms(uint32_t ms)
{
    tick_count = 0;
    while(tick_count < ms);
}

void SysTick_Handler()
{
    tick_count++;
    if (task_manager_started()) {
        arch_context_switch();
    }
}
