#include <stm32wb55xx.h>
#include <stm32wb55xx/gpio.h>
#include <stm32wb55xx/lpuart.h>
#include <stm32wb55xx/i2c.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/debug/log.h>
#include <drivers/sensor/accelerometer/mma8451.h>
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
    mma8451_configuration_t accelerometer;
    mma8451_handle_t accel_handle;

    __enable_irq();

    // TODO: Handle this within the driver
    rcc_enable_lpuart1_clock();
    rcc_enable_i2c1_clock();
    rcc_enable_gpioa_clock();
    rcc_enable_gpiob_clock();

    // Initialize lpuart interface
    lpuart_init((lpuart_configuration_t)
                {.lpuart = LPUART1,
                 .rx_port = GPIOA,
                 .rx_pin = 3,
                 .tx_port = GPIOA,
                 .tx_pin = 2,
                 .clock_source = RCC_LPUART_CLOCK_SOURCE_SYSCLK,
                 .word_length = LPUART_WORD_LENGTH_8,
                 .baud_rate_prescaler = 0x8ae3,
                 .stop_bits = LPUART_STOP_BITS_1},
                &lpuart_handle);

    // Initialize log
    log_init((log_configuration_t)
             {.lpuart_handle = lpuart_handle});

    log_debug("Accelerometer example!");


    // Initialize system timer
    system_timer_init();
    system_timer_start();

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

    // Initialize Acceleromter
    error = i2c_device_init((i2c_device_configuration_t)
                            {.i2c = I2C1,
                             .address_mode = I2C_ADDRESS_MODE_7BIT,
                             .address = 0x1d},
                            &accelerometer.i2c_handle);
    if (error) {
        log_error(error, "Failed to initialize I2C device for acceleromter");
        while(1);
    }

    error = mma8451_init(accelerometer, &accel_handle);
    if (error) {
        log_error(error, "Failed to initialize acceleromter");
        while(1);
    }


    while (1) {
        uint32_t x, y, z;

        error = mma8451_read(accel_handle, &x, &y, &z);
        if (error) {
            log_error(error, "Failed to read data from accelerometer");
            while(1);
        }

        log_info("X: %u  Y: %u  Z: %u", x, y, z);
        system_timer_wait_ms(1000);
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
    rcc_set_msi_clock_speed(RCC_MSI_CLOCK_SPEED_16MHz);
    rcc_set_system_clock_source(RCC_SYSTEM_CLOCK_SOURCE_MSI);
    rcc_disable_interrupts();

    main();
}
