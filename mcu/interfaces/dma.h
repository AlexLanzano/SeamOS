#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include <libraries/error.h>

typedef enum dma_mode {
    DMA_MODE_MEMORY_TO_MEMORY,
    DMA_MODE_MEMORY_TO_INTERFACE,
    DMA_MODE_INTERFACE_TO_MEMORY,
    DMA_MODE_INTERFACE_TO_INTERFACE
} dma_mode_t;

typedef struct dma_configuration {
    dma_mode_t mode;
    dma_interface_t interface;
    bool increment_source;
    bool increment_destination;
    void *mcu_config;
} dma_configuration_t;

error_t dma_request(dma_configuration_t *config, uint32_t dest, uint32_t src, uint32_t size, void (*callback)());

#endif
