#ifndef UART_H
#define UART_H

error_t uart_init(uint32_t handle, void *config);
error_t uart_deinit(uint32_t handle);
error_t uart_read(uint32_t handle, uint8_t *data, uint32_t length);
error_t uart_write(uint32_t handle, uint8_t *data, uint32_t length);

#endif
