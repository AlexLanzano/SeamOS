#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdint.h>
#include <libraries/error.h>

typedef uint32_t semaphore_handle_t;

error_t semaphore_init(uint32_t max_count, semaphore_handle_t *handle);
error_t semaphore_deinit(semaphore_handle_t handle);
error_t semaphore_lock(semaphore_handle_t handle);
error_t semaphore_release(semaphore_handle_t handle);

#endif
