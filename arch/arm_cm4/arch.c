#include <kernel/debug/log.h>
#include <arch/arch.h>
#include <arch/arm_cm4/cmsis_gcc.h>

#define SBC_ICSR (*((volatile uint32_t *)0xe000ed04))
#define PENDSVSET_BIT (1UL << 28UL)

uint32_t *arch_context_init(uint32_t *stack, void (*task_entry)(void))
{
    uint32_t *stack_top = stack;

    stack_top--;
    *stack_top = 0x01000000; // xPSR
    stack_top--;
    *stack_top = (uint32_t)task_entry & 0xfffffffe; // PC
    stack_top--;
    *stack_top = 0xfffffffd; // LR
    stack_top -= 5; // R12, R3, R2, R1
    *stack_top = 0; // RO
    stack_top--;
    *stack_top = 0xfffffffd;
    stack_top -= 8; // R11, R10, R9, R8, R7, R6, R5, R4

    return stack_top;
}

void arch_context_switch()
{
    SBC_ICSR |= PENDSVSET_BIT;
}

void __attribute__((naked)) arch_start_first_task()
{
    asm volatile("    mov r0, #0    \n"
                 "    msr control, r0    \n"
                 "    cpsie i    \n"
                 "    cpsie f    \n"
                 "    dsb\n"
                 "    isb\n"
                 "    svc 0 \n"
                 ::);
}

void arch_enable_irq()
{
    __enable_irq();
}

void arch_disable_irq()
{
    __disable_irq();
}

void __attribute__((naked)) SVC_Handler()
{
    asm volatile("    ldr r3, current_task2    \n"
                 "    ldr r0, [r3]             \n"
                 "    add r0, #36              \n"
                 "    msr psp, r0              \n"
                 "    isb                      \n"
                 "    pop {r0-r5}              \n"
                 "    mov lr, r5               \n"
                 "    pop {r3}                 \n"
                 "    pop {r2}                 \n"
                 "    ldr lr, address\n"
                 "    bx lr                    \n"
                 "                             \n"
                 "    .align 4                 \n"
                 "current_task2: .word g_current_task \n"
                 "address: .word 0xfffffffd \n"
                 ::);
}

void __attribute__((naked)) PendSV_Handler()
{
    asm volatile("    mrs r0, psp               \n" // Save the task context
                 "    isb                       \n"
                 "    ldr r3, current_task      \n"
                 "    add r4, #1                \n"
                 "    stmdb r0!, {r4-r11, r14}  \n"
                 "    str r0, [r3]              \n"
                 "                              \n"
                 "    stmdb sp!, {r0, r3}       \n" // Schedule the next task
                 "    bl task_manager_schedule  \n"
                 "    ldmia sp!, {r0, r3}       \n"
                 "                              \n"
                 "    ldr r0, [r3]              \n" // Restore the task context
                 "    ldmia r0!, {r4-r11, r14}  \n"
                 "    msr psp, r0               \n"
                 "    bx r14                    \n"
                 "                              \n"
                 "    .align 4                  \n"
                 "current_task: .word g_current_task \n"
                 ::);
}
