#include <stdint.h>
#include <stdbool.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/dma.h>
#include <libraries/error.h>
#include <libraries/string.h>

#define DMA_CHANNEL_MAX 7

typedef struct dma_channel {
    bool is_initialized;
    dma_configuration_t config;
} dma_channel_t;

dma_channel_t g_dma_channels[DMA_CHANNEL_MAX];

static bool dma_invalid_handle(dma_handle_t handle)
{
    return (handle >= DMA_CHANNEL_MAX || !g_dma_channels[handle].is_initialized);
}

static error_t dma_find_free_channel(dma_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    dma_handle_t dma_handle;
    for (dma_handle = 0; dma_handle < DMA_CHANNEL_MAX; dma_handle++) {
        if (!g_dma_channels[dma_handle].is_initialized) {
            *handle = dma_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static DMA_Channel_TypeDef *dma_get_channel_from_handle(const dma_handle_t handle)
{
    if (handle >= DMA_CHANNEL_MAX) {
        return 0;
    }

    return (DMA_Channel_TypeDef *)(DMA1_Channel1_BASE + (sizeof(DMA_Channel_TypeDef) * handle));
}

static DMAMUX_Channel_TypeDef *dma_get_mux_channel_from_handle(const dma_handle_t handle)
{
    if (handle >= DMA_CHANNEL_MAX) {
        return 0;
    }

    return (DMAMUX_Channel_TypeDef *)(DMAMUX1_Channel0_BASE + (sizeof(DMAMUX_Channel_TypeDef) * handle));
}

static error_t dma_configure_channel(const dma_handle_t handle,
                                     const uint32_t dest,
                                     const uint32_t src,
                                     const uint32_t length)
{
    if (dma_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    DMA_Channel_TypeDef *dma_channel = dma_get_channel_from_handle(handle);
    dma_configuration_t config = g_dma_channels[handle].config;

    switch (config.mode) {
    case DMA_MODE_MEMORY_TO_MEMORY:
        dma_channel->CCR |= DMA_CCR_MEM2MEM |
                           (config.increment_source << DMA_CCR_PINC_Pos) |
                           (config.increment_destination << DMA_CCR_MINC_Pos);
        dma_channel->CPAR = src;
        dma_channel->CMAR = dest;
        break;
    case DMA_MODE_MEMORY_TO_PERIPHERAL:
        dma_channel->CCR |= DMA_CCR_DIR |
                           (config.increment_source << DMA_CCR_MINC_Pos) |
                           (config.increment_destination << DMA_CCR_PINC_Pos);
        dma_channel->CPAR = dest;
        dma_channel->CMAR = src;
        break;
    case DMA_MODE_PERIPHERAL_TO_MEMORY:
        dma_channel->CCR |= (config.increment_source << DMA_CCR_PINC_Pos) |
                            (config.increment_destination << DMA_CCR_MINC_Pos);
        dma_channel->CPAR = src;
        dma_channel->CMAR = dest;
        break;
    case DMA_MODE_PERIPHERAL_TO_PERIPHERAL:
        dma_channel->CCR |= (config.increment_source << DMA_CCR_MINC_Pos) |
                            (config.increment_destination << DMA_CCR_PINC_Pos);
        dma_channel->CPAR = dest;
        dma_channel->CMAR = src;
        break;
    }

    NVIC_EnableIRQ(DMA1_Channel1_IRQn + handle);
    if (config.half_transfer_handler) {
        dma_channel->CCR |= DMA_CCR_TCIE;
    }

    if (config.transfer_complete_handler) {
        dma_channel->CCR |= DMA_CCR_HTIE;
    }

    if (config.transfer_error_handler) {
        dma_channel->CCR |= DMA_CCR_TEIE;
    }

    dma_channel->CNDTR = length;
    return SUCCESS;
}

error_t dma_init(dma_configuration_t config, dma_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;
    DMAMUX_Channel_TypeDef *dmamux;

    error = dma_find_free_channel(handle);
    if (error) {
        return error;
    }

    memcpy(&g_dma_channels[*handle].config, &config, sizeof(dma_configuration_t));

    dmamux = dma_get_mux_channel_from_handle(*handle);
    dmamux->CCR |= config.request << DMAMUX_CxCR_DMAREQ_ID_Pos;

    g_dma_channels[*handle].is_initialized = true;

    return SUCCESS;
}
error_t dma_start(dma_handle_t handle, uint32_t dest, uint32_t src, uint32_t size)
{
    if (dma_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    DMA_Channel_TypeDef *dma_channel = dma_get_channel_from_handle(handle);
    while (dma_channel->CCR & DMA_CCR_EN);
    dma_configure_channel(handle, dest, src, size);
    dma_channel->CCR |= DMA_CCR_EN;
    return SUCCESS;
}

error_t dma_stop(dma_handle_t handle)
{
    if (dma_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    DMA_Channel_TypeDef *dma_channel = dma_get_channel_from_handle(handle);

    dma_channel->CCR &= ~DMA_CCR_EN;
    NVIC_DisableIRQ(DMA1_Channel1_IRQn + handle);

    return SUCCESS;
}

void DMA1_Channel1_IRQHandler()
{
    dma_configuration_t *config = &g_dma_channels[0].config;
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        config->transfer_complete_handler(0);
    }
}
