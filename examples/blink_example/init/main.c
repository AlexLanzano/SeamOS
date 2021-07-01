#include <config.h>
#include <stm32wb55xx.h>
#include <stm32wb55xx/gpio.h>
#include <stm32wb55xx/lpuart.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <libraries/queue.h>
#include <libraries/error.h>
#include <applications/blink.h>

extern uint32_t _estack[];
extern uint32_t _bootloader_magic[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];
extern uint32_t interrupt_vector_table[];

gpio_handle_t g_led_handle;
extern uint32_t g_blink_stack[];

void main()
{
    task_handle_t blink_task_handle;
    lpuart_configuration_t lpuart_config;
    lpuart_handle_t lpuart_handle;
    log_configuration_t log;

    // TODO: Handle this within the driver
    rcc_enable_lpuart1_clock();
    rcc_enable_gpioa_clock();
    rcc_enable_gpiob_clock();

    // Setup lpuart interface for log
    lpuart_config.lpuart = LPUART1;
    lpuart_config.rx_port = GPIOA;
    lpuart_config.rx_pin = 3;
    lpuart_config.tx_port = GPIOA;
    lpuart_config.tx_pin = 2;
    lpuart_config.clock_source = RCC_LPUART_CLOCK_SOURCE_SYSCLK;
    lpuart_config.word_length = LPUART_WORD_LENGTH_8;
    lpuart_config.baud_rate_prescaler = 0x115c7;
    lpuart_config.stop_bits = LPUART_STOP_BITS_1;
    lpuart_init(lpuart_config, &lpuart_handle);

    log.lpuart_handle = lpuart_handle;
    log_init(log);

    log_info("Start");

    task_manager_init();
    task_manager_init_task(&blink_task_entry,
                           1,
                           500,
                           g_blink_stack,
                           &blink_task_handle);

    task_manager_start();
}

void __attribute__((naked)) Reset_Handler()
{
    __asm__("ldr r0, =_estack\n\t"
            "mov sp, r0");

    // Copy data section from flash memory to ram
    uint32_t data_section_size = _edata - _sdata;
    memcpy(_sdata, _sidata, data_section_size*4);

    // Zero out bss
    uint32_t bss_section_size = _ebss - _sbss;
    memset(_sbss, 0, bss_section_size*4);

    // Set Interrupt Vector Table Offset
    SCB->VTOR = (uint32_t)interrupt_vector_table;

    rcc_reset();
    rcc_set_msi_clock_speed(CONFIG_CLOCK_FREQ);
    rcc_set_system_clock_source(RCC_SYSTEM_CLOCK_SOURCE_MSI);
    rcc_disable_interrupts();

    main();
}

void HardFault_Handler()
{
    log_error(ERROR_FAULT, "HARD FAULT");
    while(1);
}

void BusFault_Handler()
{
    log_error(ERROR_FAULT, "BUS FAULT");
    while(1);
}

void UsageFault_Handler()
{
    log_error(ERROR_FAULT, "USEAGE FAULT");
    while(1);
}
