#include <config.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <mcu/stm32wb55xx/lpuart.h>
#include <mcu/stm32wb55xx/i2c.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <drivers/input/ft6336g.h>

extern uint32_t _estack[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];
extern uint32_t interrupt_vector_table[];

uint32_t g_touchscreen_task_stack[500] = {0};

void touchscreen_task_entry()
{
    error_t error;
    i2c_handle_t i2c_handle;
    ft6336g_handle_t ft6336g_handle;
    gpio_handle_t int_pin_handle;

    // Initialize touchscreen
    error = i2c_device_init((i2c_device_configuration_t)
                            {.i2c = I2C1,
                             .address_mode = I2C_ADDRESS_MODE_7BIT,
                             .address = 0x38},
                            &i2c_handle);
    if (error) {
        log_error(error, "Failed to initialize I2C device for touchscreen");
        while(1);
    }

    error = gpio_init((gpio_configuration_t)
                      {.port = GPIOA,
                       .pin = 0,
                       .mode = GPIO_MODE_INPUT,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_HIGH,
                       .pull_resistor = GPIO_PULL_RESISTOR_UP},
                      &int_pin_handle);
    if (error) {
        log_error(error, "Failed to initialize interrupt pin for touchscreen");
        while (1);
    }

    error = ft6336g_init((ft6336g_configuration_t)
                         {.i2c_handle = i2c_handle,
                          .threshold = 40,
                          .interrupt_mode = FT6336G_INTERRUPT_MODE_POLLING},
                         &ft6336g_handle);
    if (error) {
        log_error(error, "Failed to initialize touchscreen device");
        while (1);
    }

    uint8_t touch_not_detected;
    while (1) {
        uint16_t x, y;
        ft6336g_event_flag_t event_flag;

        gpio_read(int_pin_handle, &touch_not_detected);
        if (touch_not_detected) {
            continue;
        }

        log_info("touch detected");
        error = ft6336g_read(ft6336g_handle, &x, &y, &event_flag);
        if (error) {
            log_error(error, "Failed to read data from touch screen");
        }

        log_info("X: %u  Y: %u  Event_flag: %u", x, y, event_flag);
    }
}

void main()
{
    error_t error;
    lpuart_handle_t lpuart_handle;
    task_handle_t touchscreen_task_handle;

    // TODO: Handle this within the driver
    rcc_enable_lpuart1_clock();
    rcc_enable_i2c1_clock();
    rcc_enable_gpioa_clock();
    rcc_enable_gpiob_clock();

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

    // Initialize log
    log_init((log_configuration_t)
             {.lpuart_handle = lpuart_handle});

    log_info("Touchscreen Example!");

    // Initialize I2C interface
    error = i2c_interface_init((i2c_interface_configuration_t)
                               {.i2c = I2C1,
                                .scl_port = GPIOB,
                                .scl_pin = 6,
                                .sda_port = GPIOB,
                                .sda_pin = 7,
                                .timing = 1234});
    if (error) {
        log_error(error, "Failed to initialize I2C interface");
        while(1);
    }

    task_manager_init();
    task_manager_init_task(&touchscreen_task_entry,
                           1,
                           500,
                           g_touchscreen_task_stack,
                           &touchscreen_task_handle);
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
    rcc_set_pll_clock_speed(CONFIG_CLOCK_FREQ);
    rcc_set_system_clock_source(RCC_SYSTEM_CLOCK_SOURCE_PLL);
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
