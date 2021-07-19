#ifndef STM32WB55XX_DMA_H
#define STM32WB55XX_DMA_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/dma.h>

typedef enum dma_data_size {
    DMA_TRANSFER_SIZE_8BITS,
    DMA_TRANSFER_SIZE_16BITS,
    DMA_TRANSFER_SIZE_32BITS,
} dma_data_size_t;

typedef enum dma_mode {
    DMA_MODE_MEMORY_TO_MEMORY,
    DMA_MODE_MEMORY_TO_PERIPHERAL,
    DMA_MODE_PERIPHERAL_TO_MEMORY,
    DMA_MODE_PERIPHERAL_TO_PERIPHERAL
} dma_mode_t;

typedef enum dma_request {
    DMA_REQUEST_MEMORY_TO_MEMORY,
    DMA_REQUEST_GEN0,
    DMA_REQUEST_GEN1,
    DMA_REQUEST_GEN2,
    DMA_REQUEST_GEN3,
    DMA_REQUEST_ADC1,
    DMA_REQUEST_SPI1_RX,
    DMA_REQUEST_SPI1_TX,
    DMA_REQUEST_SPI2_RX,
    DMA_REQUEST_SPI2_TX,
    DMA_REQUEST_I2C1_RX,
    DMA_REQUEST_I2C1_TX,
    DMA_REQUEST_I2C3_RX,
    DMA_REQUEST_I2C3_TX,
    DMA_REQUEST_USART1_RX,
    DMA_REQUEST_USART1_TX,
    DMA_REQUEST_LPUART1_RX,
    DMA_REQUEST_LPUART1_TX,
    DMA_REQUEST_SAI1_A,
    DMA_REQUEST_SAI1_B,
    DMA_REQUEST_QUADSPI,
    DMA_REQUEST_TIM1_CH1,
    DMA_REQUEST_TIM1_CH2,
    DMA_REQUEST_TIM1_CH3,
    DMA_REQUEST_TIM1_CH4,
    DMA_REQUEST_TIM1_UP,
    DMA_REQUEST_TIM1_TRIG,
    DMA_REQUEST_TIM1_COM,
    DMA_REQUEST_TIM2_CH1,
    DMA_REQUEST_TIM2_CH2,
    DMA_REQUEST_TIM2_CH3,
    DMA_REQUEST_TIM2_CH4,
    DMA_REQUEST_TIM2_UP,
    DMA_REQUEST_TIM16_CH1,
    DMA_REQUEST_TIM16_UP,
    DMA_REQUEST_TIM17_CH1,
    DMA_REQUEST_TIM17_UP,
    DMA_REQUEST_AES1_IN,
    DMA_REQUEST_AES1_OUT,
    DMA_REQUEST_AES2_IN,
    DMA_REQUEST_AES2_OUT
} dma_request_t;

typedef enum dma_interrupt_status {
    DMA_INTERRUPT_STATUS_GLOBAL_INTERRUPT = 1,
    DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE = 2,
    DMA_INTERRUPT_STATUS_HALF_TRANSFER = 4,
    DMA_INTERRUPT_STATUS_TRANSFER_ERROR = 8
} dma_interrupt_status_t;

typedef struct dma_configuration {
    dma_mode_t mode;
    dma_request_t request;
    bool increment_source;
    bool increment_destination;
    void (*transfer_complete_handler)(dma_handle_t);
    void (*half_transfer_handler)(dma_handle_t);
    void (*transfer_error_handler)(dma_handle_t);
} dma_configuration_t;

#endif
