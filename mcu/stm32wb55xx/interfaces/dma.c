#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/interfaces/dma.h>
#include <mcu/interfaces/dma.h>
#include <libraries/error.h>
#include <libraries/string.h>

#define DMA_MAX_INTERFACES 2

// TODO: Add multi channel support

typedef struct dma_channel {
    DMA_Channel_TypeDef *channel;
    DMAMUX_Channel_TypeDef *mux_channel;
    void (*callback)(uint32_t cb_handle);
    uint32_t callback_handle;
} dma_channel_t;

dma_channel_t g_dma_channel = {
    .channel = DMA1_Channel1,
    .mux_channel = DMAMUX1_Channel0
};

/* typedef struct dma_interface { */
/*     dma_channel_t channels[7]; */
/* } dma_interface_t; */

/* static dma_interface_t g_dma1 = {.channels = */
/*                                  {{.channel = DMA1_Channel1, .mux_channel = DMAMUX1_Channel0}, */
/*                                   {.channel = DMA1_Channel2, .mux_channel = DMAMUX1_Channel1}, */
/*                                   {.channel = DMA1_Channel3, .mux_channel = DMAMUX1_Channel2}, */
/*                                   {.channel = DMA1_Channel4, .mux_channel = DMAMUX1_Channel3}, */
/*                                   {.channel = DMA1_Channel5, .mux_channel = DMAMUX1_Channel4}, */
/*                                   {.channel = DMA1_Channel6, .mux_channel = DMAMUX1_Channel5}, */
/*                                   {.channel = DMA1_Channel7, .mux_channel = DMAMUX1_Channel6}}}; */

/* static dma_interface_t g_dma2 = {.channels = */
/*                                  {{.channel = DMA2_Channel1, .mux_channel = DMAMUX1_Channel7}, */
/*                                   {.channel = DMA2_Channel2, .mux_channel = DMAMUX1_Channel8}, */
/*                                   {.channel = DMA2_Channel3, .mux_channel = DMAMUX1_Channel9}, */
/*                                   {.channel = DMA2_Channel4, .mux_channel = DMAMUX1_Channel10}, */
/*                                   {.channel = DMA2_Channel5, .mux_channel = DMAMUX1_Channel11}, */
/*                                   {.channel = DMA2_Channel6, .mux_channel = DMAMUX1_Channel12}, */
/*                                   {.channel = DMA2_Channel7, .mux_channel = DMAMUX1_Channel13}}}; */

/* static dma_interface_t *g_dma_interfaces[DMA_MAX_INTERFACES] = {&g_dma1, &g_dma2}; */

static bool dma_invalid_handle(uint32_t handle)
{
    return handle >= DMA_MAX_INTERFACES;
}

/* static dma_channel_t *dma_get_free_channel(uint32_t handle) */
/* { */
/*     dma_interface_t *interface = g_dma_interfaces[handle]; */
/*     uint32_t channel_index; */
/*     for (channel_index = 0; channel_index < 7; channel_index++) { */
/*         if (!interface->channels[channel_index].in_use) { */
/*             return &interface->channels[channel_index]; */
/*         } */
/*     } */

/*     return NULL; */
/* } */


static error_t dma_configure_channel(DMA_Channel_TypeDef *dma_channel,
                                     dma_configuration_t *config,
                                     const uint32_t dest,
                                     const uint32_t src,
                                     const uint32_t length)
{
    switch (config->mode) {
    case DMA_MODE_MEMORY_TO_MEMORY:
        dma_channel->CCR |= DMA_CCR_MEM2MEM |
                           (config->increment_source << DMA_CCR_PINC_Pos) |
                           (config->increment_destination << DMA_CCR_MINC_Pos);
        dma_channel->CPAR = src;
        dma_channel->CMAR = dest;
        break;
    case DMA_MODE_MEMORY_TO_INTERFACE:
        dma_channel->CCR |= DMA_CCR_DIR |
                           (config->increment_source << DMA_CCR_MINC_Pos) |
                           (config->increment_destination << DMA_CCR_PINC_Pos);
        dma_channel->CPAR = dest;
        dma_channel->CMAR = src;
        break;
    case DMA_MODE_INTERFACE_TO_MEMORY:
        dma_channel->CCR |= (config->increment_source << DMA_CCR_PINC_Pos) |
                            (config->increment_destination << DMA_CCR_MINC_Pos);
        dma_channel->CPAR = src;
        dma_channel->CMAR = dest;
        break;
    case DMA_MODE_INTERFACE_TO_INTERFACE:
        dma_channel->CCR |= (config->increment_source << DMA_CCR_MINC_Pos) |
                            (config->increment_destination << DMA_CCR_PINC_Pos);
        dma_channel->CPAR = dest;
        dma_channel->CMAR = src;
        break;
    }

    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    dma_channel->CCR |= DMA_CCR_TCIE;
    dma_channel->CCR |= DMA_CCR_TEIE;

    dma_channel->CNDTR = length;
    return SUCCESS;
}

static dma_request_id_t dma_get_request_id(uint32_t *addr)
{
    if (addr == &SPI1->DR) {
        return DMA_REQUEST_ID_SPI1_TX;
    } else {
        // TODO: Implement more interfaces
    }
    return DMA_REQUEST_ID_MEMORY_TO_MEMORY;
}

error_t dma_request(uint32_t handle, dma_configuration_t *config, void *dest, void *src, uint32_t size,
                    void (*callback)(uint32_t handle), uint32_t cb_handle)
{
    if (dma_invalid_handle(handle) || !config || !callback) {
        return ERROR_INVALID;
    }

    volatile dma_channel_t *channel = &g_dma_channel;
    // TODO: add timeout
    while (channel->channel->CCR & DMA_CCR_EN);

    if (config->mode == DMA_MODE_MEMORY_TO_INTERFACE) {
        channel->mux_channel->CCR |= dma_get_request_id(dest) << DMAMUX_CxCR_DMAREQ_ID_Pos;
    } else if (config->mode == DMA_MODE_INTERFACE_TO_MEMORY) {
        channel->mux_channel->CCR |= dma_get_request_id(src) << DMAMUX_CxCR_DMAREQ_ID_Pos;
    }

    channel->callback = callback;
    channel->callback_handle = cb_handle;

    dma_configure_channel(channel->channel, config, (uint32_t)dest, (uint32_t)src, size);
    channel->channel->CCR |= DMA_CCR_EN;
    return SUCCESS;
}

/* void dma_irq_handler(dma_interface_t *dma, uint32_t channel) */
/* { */
/*     dma->channels[channel].in_use = false; */
/*     dma->channels[channel].channel->CCR &= ~DMA_CCR_EN; */
/*     dma_disable_channel_irq(dma->channels[channel].channel); */
/*     dma->channels[channel].callback(dma->channels[channel].callback_handle); */
/* } */

void DMA1_Channel1_IRQHandler()
{
    if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE ||
        DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR)
    {
        DMA1->IFCR = 0xffffffff;
        g_dma_channel.channel->CCR &= ~DMA_CCR_EN;
        g_dma_channel.callback(g_dma_channel.callback_handle);
        NVIC_DisableIRQ(DMA1_Channel1_IRQn);
        //dma_irq_handler(&g_dma1, 0);

    }
}

/* void DMA1_Channel2_IRQHandler() */
/* { */
/*     if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA1->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma1, 1); */
/*     } */
/* } */

/* void DMA1_Channel3_IRQHandler() */
/* { */
/*     if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA1->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma1, 2); */
/*     } */
/* } */

/* void DMA1_Channel4_IRQHandler() */
/* { */
/*     if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA1->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma1, 3); */
/*     } */
/* } */

/* void DMA1_Channel5_IRQHandler() */
/* { */
/*     if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA1->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma1, 4); */
/*     } */
/* } */

/* void DMA1_Channel6_IRQHandler() */
/* { */
/*     if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA1->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma1, 5); */
/*     } */
/* } */

/* void DMA1_Channel7_IRQHandler() */
/* { */
/*     if (DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA1->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA1->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma1, 6); */
/*     } */
/* } */

/* void DMA2_Channel1_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 0); */
/*     } */
/* } */

/* void DMA2_Channel2_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 1); */
/*     } */
/* } */

/* void DMA2_Channel3_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 2); */
/*     } */
/* } */

/* void DMA2_Channel4_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 3); */
/*     } */
/* } */

/* void DMA2_Channel5_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 4); */
/*     } */
/* } */

/* void DMA2_Channel6_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 5); */
/*     } */
/* } */

/* void DMA2_Channel7_IRQHandler() */
/* { */
/*     if (DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_COMPLETE || */
/*         DMA2->ISR & DMA_INTERRUPT_STATUS_TRANSFER_ERROR) { */
/*         DMA2->IFCR = 0xffffffff; */
/*         dma_irq_handler(&g_dma2, 6); */
/*     } */
/* } */
