#include <config.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <mcu/stm32wb55xx/lpuart.h>
#include <mcu/stm32wb55xx/spi.h>
#include <mcu/stm32wb55xx/dma.h>
#include <mcu/system_timer.h>
#include <kernel/task/task_manager.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <drivers/display/st7789.h>

extern uint32_t _estack[];
extern uint32_t _bootloader_magic[];
extern uint32_t _sidata[];
extern uint32_t _sdata[];
extern uint32_t _edata[];
extern uint32_t _sbss[];
extern uint32_t _ebss[];
extern uint32_t interrupt_vector_table[];

uint32_t g_display_task_stack[700] = {0};

void dma_transfer_complete_handler(dma_handle_t handle)
{
    dma_stop(handle);
    DMA1->IFCR |= 0xfffffff;
}

void display_task_entry()
{
    error_t error;
    spi_device_handle_t display_spi_handle;
    gpio_handle_t dc_pin_handle;
    st7789_handle_t display_handle;
    dma_handle_t spi_dma_handle;

    // Initialize display
    error = dma_init((dma_configuration_t)
                     {.mode = DMA_MODE_MEMORY_TO_PERIPHERAL,
                      .request = DMA_REQUEST_SPI1_TX,
                      .increment_source = true,
                      .transfer_complete_handler = &dma_transfer_complete_handler},
                     &spi_dma_handle);

    error = spi_device_init((spi_device_configuration_t)
                            {.spi = SPI1,
                             .cs_port = GPIOB,
                             .cs_pin = 1,
                             .clock_mode = SPI_CLOCK_MODE_0,
                             .mode = SPI_MODE_MASTER,
                             .baud_rate_prescaler = SPI_BAUD_RATE_PRESCALER_2,
                             .significant_bit = SPI_SIGNIFICANT_BIT_MSB,
                             .com_mode = SPI_COM_MODE_FULL_DUPLEX,
                             .data_size = SPI_DATA_SIZE_8BIT,
                             .active_low = true,
                             .dma_handle = spi_dma_handle},
                            &display_spi_handle);
    if (error) {
        log_error(error, "Failed to configure spi device for display device");
        while(1);
    }

    error = gpio_init((gpio_configuration_t)
                      {.port = GPIOB,
                       .pin = 0,
                       .mode = GPIO_MODE_OUTPUT,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_HIGH,
                       .pull_resistor = GPIO_PULL_RESISTOR_NONE},
                      &dc_pin_handle);

    error = st7789_init((st7789_configuration_t)
                        {.spi_handle = display_spi_handle,
                         .dc_pin_handle = dc_pin_handle,
                         .framebuffer_size = 240 * 240,
                         .color_format = ST7789_COLOR_FORMAT_16_BIT,
                         .width = 240,
                         .height = 240},
                        &display_handle);
    if (error) {
        log_error(error, "Failed to initialize display");
        while(1);
    }

    st7789_clear(display_handle);
    task_manager_task_wait_ms(500);
    while (1) {
        st7789_draw_filled_rect(display_handle, 0, 0, 240, 240, 0xF800);
        task_manager_task_wait_ms(250);
        st7789_draw_filled_rect(display_handle, 0, 0, 240, 240, 0x07E0);
        task_manager_task_wait_ms(250);
        st7789_draw_filled_rect(display_handle, 0, 0, 240, 240, 0x001F);
        task_manager_task_wait_ms(250);
    }
}

void main()
{
    error_t error;
    lpuart_handle_t lpuart_handle;
    spi_interface_handle_t spi_interface_handle;
    task_handle_t display_task_handle;

    // TODO: Handle this within the driver
    rcc_enable_lpuart1_clock();
    rcc_enable_spi1_clock();
    rcc_enable_dma1_clock();
    rcc_enable_dmamux1_clock();

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

    log_info("Display Example!");

    // Initialize SPI interface
    error = spi_interface_init((spi_interface_configuration_t)
                               {.sck_port = GPIOA,
                                .sck_pin = 5,
                                .miso_port = GPIOA,
                                .miso_pin = 6,
                                .mosi_port = GPIOA,
                                .mosi_pin = 7},
                               &spi_interface_handle);
    if (error) {
        log_error(error, "Failed to initialize spi interface");
        while(1);
    }

    task_manager_init();
    task_manager_init_task(&display_task_entry,
                           1,
                           500,
                           g_display_task_stack,
                           &display_task_handle);
    task_manager_start();
}

void init()
{
    g_stm32wb55xx_lpuart1_config.clock_source = RCC_LPUART_CLOCK_SOURCE_SYSCLK;
    g_stm32wb55xx_lpuart1_config.word_length = LPUART_WORD_LENGTH_8;
    g_stm32wb55xx_lpuart1_config.stop_bits = LPUART_STOP_BITS_1;

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
