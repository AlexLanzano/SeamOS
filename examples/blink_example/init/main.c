#include <stm32wb55xx.h>
#include <stm32wb55xx/gpio.h>
#include <stm32wb55xx/lpuart.h>
#include <stm32wb55xx/systick_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <applications/blink.h>

extern uint32_t _estack[];
extern uint32_t _bootloader_magic[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];
extern uint32_t interrupt_vector_table[];

void main()
{
    blink_application_data_t data;
    lpuart_configuration_t lpuart_config;
    lpuart_handle_t lpuart_handle;
    gpio_configuration_t pin;
    log_configuration_t log;

    __enable_irq();

    // TODO: Handle this within the driver
    rcc_enable_lpuart1_clock();
    rcc_enable_gpioa_clock();
    rcc_enable_gpiob_clock();

    pin.port = GPIOA;
    pin.pin = 0;
    pin.mode = GPIO_MODE_OUTPUT;
    pin.output_type = GPIO_OUTPUT_TYPE_PUSH_PULL;
    pin.output_speed = GPIO_OUTPUT_SPEED_LOW;
    pin.pull_resistor = GPIO_PULL_RESISTOR_NONE;
    gpio_configure_pin(pin, &data.gpio_handle);

    // Setup lpuart interface for log
    lpuart_config.lpuart = LPUART1;
    lpuart_config.rx_port = GPIOA;
    lpuart_config.rx_pin = 3;
    lpuart_config.tx_port = GPIOA;
    lpuart_config.tx_pin = 2;
    lpuart_config.clock_source = RCC_LPUART_CLOCK_SOURCE_SYSCLK;
    lpuart_config.word_length = LPUART_WORD_LENGTH_8;
    lpuart_config.baud_rate_prescaler = 0x8ae3;
    lpuart_config.stop_bits = LPUART_STOP_BITS_1;
    lpuart_init(lpuart_config, &lpuart_handle);

    log.lpuart_handle = lpuart_handle;
    log_init(log);

    systick_timer_init();
    systick_timer_start();

    task_manager_init();
    task_manager_add_task(string("blink"), &blink_application_start, &data);
    task_manager_start_task_by_name(string("blink"));
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
    rcc_set_msi_clock_speed(RCC_MSI_CLOCK_SPEED_16MHz);
    rcc_set_system_clock_source(RCC_SYSTEM_CLOCK_SOURCE_MSI);
    rcc_disable_interrupts();

    main();
}
