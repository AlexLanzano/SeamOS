#ifndef LPUART_H
#define LPUART_H

#include <stdint.h>
#include <stdbool.h>
#include <libraries/error.h>

typedef struct lpuart_configuration  lpuart_configuration_t;
typedef uint32_t lpuart_handle_t;

error_t lpuart_read(lpuart_handle_t handle, uint8_t *data);
error_t lpuart_write(lpuart_handle_t handle, uint8_t *data, uint32_t length);
error_t lpuart_init(lpuart_configuration_t config, lpuart_handle_t *handle);

#endif
