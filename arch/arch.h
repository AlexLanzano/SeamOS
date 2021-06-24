#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>

uint32_t *arch_context_init();
void arch_context_switch();
void arch_start_first_task();
void arch_enable_irq();
void arch_disable_irq();

#endif
