#include <config.h>
#include <libraries/string.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <mcu/stm32wb55xx/interfaces/uart.h>
#include <mcu/stm32wb55xx/interfaces/spi.h>
#include <mcu/stm32wb55xx//interfaces/i2c.h>

extern uint32_t _estack[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];
extern uint32_t interrupt_vector_table[];

extern void main();

static void init_peripherals()
{
    #ifdef CONFIG_ENABLE_GPIO_A
    rcc_enable_gpioa_clock();
    #endif

    #ifdef CONFIG_ENABLE_GPIO_B
    rcc_enable_gpiob_clock();
    #endif

    #ifdef CONFIG_ENABLE_LPUART1
    rcc_enable_lpuart1_clock();
    uart_enable(LPUART1,
                CONFIG_PIN_LPUART1_TX,
                CONFIG_PIN_LPUART1_RX,
                CONFIG_LPUART1_BAUD,
                CONFIG_LPUART1_WORD_LENGTH);
    #endif

    #ifdef CONFIG_ENABLE_SPI1
    rcc_enable_spi1_clock();
    spi_enable(SPI1,
               CONFIG_PIN_SPI1_SCK,
               CONFIG_PIN_SPI1_MISO,
               CONFIG_PIN_SPI1_MOSI);
    #endif

    #ifdef CONFIG_ENABLE_SPI2
    rcc_enable_spi2_clock();
    spi_enable(SPI2,
               CONFIG_PIN_SPI2_SCK,
               CONFIG_PIN_SPI2_MISO,
               CONFIG_PIN_SPI2_MOSI);
    #endif

    #ifdef CONFIG_ENABLE_I2C1
    rcc_enable_i2c1_clock();
    i2c_enable(I2C1,
               CONFIG_PIN_I2C1_SCL,
               CONFIG_PIN_I2C1_SDA,
               CONFIG_I2C1_TIMING);
    #endif

    #ifdef CONFIG_ENABLE_SAI1
    rcc_enable_sai1_clock();
    sai1_interface_init(SAI1,
                        CONFIG_PIN_SAI1_MCLK,
                        CONFIG_PIN_SAI1_SCK,
                        CONFIG_PIN_SAI1_SD,
                        CONFIG_PIN_SAI1_FS);
    #endif

    #ifdef CONFIG_ENABLE_DMAMUX1
    rcc_enable_dmamux1_clock();
    #endif

    #ifdef CONFIG_ENABLE_DMA1
    rcc_enable_dma1_clock();
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    NVIC_EnableIRQ(DMA1_Channel4_IRQn);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);
    NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    NVIC_EnableIRQ(DMA1_Channel7_IRQn);
    #endif

    #ifdef CONFIG_ENABLE_DMA2
    NVIC_EnableIRQ(DMA2_Channel1_IRQn);
    NVIC_EnableIRQ(DMA2_Channel2_IRQn);
    NVIC_EnableIRQ(DMA2_Channel3_IRQn);
    NVIC_EnableIRQ(DMA2_Channel4_IRQn);
    NVIC_EnableIRQ(DMA2_Channel5_IRQn);
    NVIC_EnableIRQ(DMA2_Channel6_IRQn);
    NVIC_EnableIRQ(DMA2_Channel7_IRQn);
    #endif


}

static void init_clock()
{
    rcc_reset();
    rcc_set_system_clock_source(CONFIG_CLOCK_SOURCE);

    switch (CONFIG_CLOCK_SOURCE) {
    case RCC_SYSTEM_CLOCK_SOURCE_MSI:
        rcc_set_msi_clock_speed(CONFIG_CLOCK_FREQ);
        break;
    case RCC_SYSTEM_CLOCK_SOURCE_PLL:
        rcc_set_pll_clock_speed(CONFIG_CLOCK_FREQ);
        break;
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

    #ifdef CONFIG_PRE_INIT
    pre_init();
    #endif

    init_clock();
    init_peripherals();

    rcc_disable_interrupts();
    main();
}
