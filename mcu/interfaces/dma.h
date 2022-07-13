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
    bool increment_source;
    bool increment_destination;
    void *mcu_config;
} dma_configuration_t;

error_t dma_request(uint32_t handle, dma_configuration_t *config, void *dest, void *src, uint32_t size,
                    void (*callback)(uint32_t handle), uint32_t cb_handle);

#endif
