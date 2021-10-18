#include <config.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <mcu/stm32wb55xx/lpuart.h>
#include <mcu/stm32wb55xx/sai.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>

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
    error_t error;
    lpuart_handle_t lpuart_handle;
    sai_interface_handle_t sai_interface_handle;
    sai_device_handle_t sai_device_handle;

    // TODO: Handle this within the driver
    rcc_enable_lpuart1_clock();
    rcc_enable_gpioa_clock();
    //rcc_enable_gpiob_clock();
    rcc_enable_sai1_clock();

    // Initialize LPUART interface
    lpuart_init((lpuart_configuration_t)
                {.lpuart = LPUART1,
                 .rx_port = GPIOA,
                 .rx_pin = 3,
                 .tx_port = GPIOA,
                 .tx_pin = 2,
                 .clock_source = RCC_LPUART_CLOCK_SOURCE_SYSCLK,
                 .word_length = LPUART_WORD_LENGTH_8,
                 .baud_rate_prescaler = 0x22b8e,
                 .stop_bits = LPUART_STOP_BITS_1},
                &lpuart_handle);

    /* // Initialize log */
    log_init((log_configuration_t)
             {.lpuart_handle = lpuart_handle});

    log_info("Mic Example!");

    // Initialize SAI interface
    error = sai_interface_init((sai_interface_configuration_t)
                               {.sck_port = GPIOA,
                                .sck_pin = 8,
                                .sd_port = GPIOA,
                                .sd_pin = 10,
                                .fs_port = GPIOA,
                                .fs_pin = 9},
                               &sai_interface_handle);
    if (error) {
        log_error(error, "Failed to initialize sai interface");
        while(1);
    }

    // Initialize Mic device
    error = sai_device_init((sai_device_configuration_t)
                            {.sai = SAI1,
                             .block = SAI1_Block_A,
                             .mode = SAI_MODE_MASTER_RECEIVER,
                             .protocol = SAI_PROTOCOL_FREE,
                             .data_size = SAI_DATA_SIZE_24BITS,
                             .lsb_first = false,
                             .frequency = 16000,
                             .clock_strobe_edge = SAI_CLOCK_STROBE_EDGE_RISING,
                             .frame_definition = SAI_FRAME_DEFINITION_SPLIT,
                             .frame_length = 64,
                             .sample_length = 32,
                             .first_bit_offset = 0,
                             .slot_count = 2,
                             .slot_size = SAI_SLOT_SIZE_32BIT,
                             .slot_enable = 0xffff},
                            &sai_device_handle);
    if (error) {
        log_error(error, "Failed to initialize sai device");
        while (1);
    }

    while (1) {
        int32_t sample = 0;
        sai_read(sai_device_handle, &sample, sizeof(int32_t));
        log_info("%x", sample);
    }
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
    // TODO: Make this into a better interface
    rcc_set_pllp_division_factor(2);
    rcc_set_pll_clock_speed(CONFIG_CLOCK_FREQ);
    rcc_set_system_clock_source(RCC_SYSTEM_CLOCK_SOURCE_PLL);
    rcc_set_sai1_clock_source(RCC_SAI_CLOCK_SOURCE_PLL);
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
