#ifndef DMA_H
#define DMA_H

#include <stdint.h>
#include <libraries/error.h>

typedef struct dma_configuration dma_configuration_t;
typedef uint32_t dma_handle_t;

error_t dma_init(dma_configuration_t config, dma_handle_t *handle);
error_t dma_deinit(dma_handle_t handle);
error_t dma_start(dma_handle_t handle, uint32_t dest, uint32_t src, uint32_t size);
error_t dma_stop(dma_handle_t handle);

#endif
