#include <stdint.h>
#include <stdbool.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/dma.h>
#include <libraries/error.h>
#include <libraries/string.h>

typedef struct dma_channel {
    bool in_use;
    DMA_Channel_TypeDef *channel;
    DMAMUX_Channel_TypeDef *mux_channel;
    void (*callback)();
} dma_channel_t;

typedef struct dma_interface {
    dma_channel_t *channels[7];
} dma_interface_t;

static dma_interface_t g_dma1 = {.channels =
                                 {{.channel = DMA1_Channel1_BASE, .mux_channel  DMAMUX1_Channel0_BASE},
                                  {.channel = DMA1_Channel2_BASE, .mux_channel  DMAMUX1_Channel1_BASE},
                                  {.channel = DMA1_Channel3_BASE, .mux_channel  DMAMUX1_Channel2_BASE},
                                  {.channel = DMA1_Channel4_BASE, .mux_channel  DMAMUX1_Channel3_BASE},
                                  {.channel = DMA1_Channel5_BASE, .mux_channel  DMAMUX1_Channel4_BASE},
                                  {.channel = DMA1_Channel6_BASE, .mux_channel  DMAMUX1_Channel5_BASE},
                                  {.channel = DMA1_Channel7_BASE, .mux_channel  DMAMUX1_Channel6_BASE}}};

static dma_interface_t g_dma2 = {.channels =
                                 {{.channel = DMA2_Channel1_BASE, .mux_channel  DMAMUX1_Channel7_BASE},
                                  {.channel = DMA2_Channel2_BASE, .mux_channel  DMAMUX1_Channel8_BASE},
                                  {.channel = DMA2_Channel3_BASE, .mux_channel  DMAMUX1_Channel9_BASE},
                                  {.channel = DMA2_Channel4_BASE, .mux_channel  DMAMUX1_Channel10_BASE},
                                  {.channel = DMA2_Channel5_BASE, .mux_channel  DMAMUX1_Channel11_BASE},
                                  {.channel = DMA2_Channel6_BASE, .mux_channel  DMAMUX1_Channel12_BASE},
                                  {.channel = DMA2_Channel7_BASE, .mux_channel  DMAMUX1_Channel13_BASE}}};

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

static dma_request_id_t dma_get_request_id(uint32_t *addr)
{
    if (addr == &SPI1->DR) {
        return DMA_REQUEST_ID_SPI1_TX;
    } else {
        // TODO: Implement more interfaces
        return DMA_REQUEST_MEMORY_TO_MEMORY;
    }
}

error_t dma_request(dma_configuration_t *config, uint32_t *dest, uint32_t *src, uint32_t size, void (*callback)())
{
    if (!config || !callback) {
        return ERROR_INVALID;
    }

    dma_channel_t channel = dma_get_free_channel();
    if (!channel) {
        // TODO: implement queue
        return ERROR_NO_MEMORY;
    }

    channel->in_use = true;
    if (config->mode == DMA_MODE_MEMORY_TO_INTERFACE) {
        channel->mux_channel->CxCR |= dma_get_request_id(dest) << DMAMUX_CxCR_DMAREQ_ID_Pos;
    } else if (config->mode == DMA_MODE_INTERFACE_TO_MEMORY) {
        channel->mux_channel->CxCR |= dma_get_request_id(src) << DMAMUX_CxCR_DMAREQ_ID_Pos;
    }

    dma_configure_channel(config, (uint32_t)dest, (uint32_)src, size);
    dma_channel->CCR |= DMA_CCR_EN;
    return SUCCESS;
}

void dma_irq_handler(dma_interface_t *dma, uint32_t channel)
{
    dma->channels[channel]->in_use = false;
    dma->channels[channel]->channel->CCR &= ~DMA_CCR_EN;
    dma->channels[channel]->callback();
}

void DMA1_Channel1_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 0);
    }
}

void DMA1_Channel2_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 1);
    }
}

void DMA1_Channel3_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 2);
    }
}

void DMA1_Channel4_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 3);
    }
}

void DMA1_Channel5_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 4);
    }
}

void DMA1_Channel6_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 5);
    }
}

void DMA1_Channel7_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma1, 6);
    }
}

void DMA2_Channel1_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 0);
    }
}

void DMA2_Channel2_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 1);
    }
}

void DMA2_Channel3_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 2);
    }
}

void DMA2_Channel4_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 3);
    }
}

void DMA2_Channel5_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 4);
    }
}

void DMA2_Channel6_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 5);
    }
}

void DMA2_Channel7_IRQHandler()
{
    if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) {
        dma_irq_handler(&g_dma2, 6);
    }
}
