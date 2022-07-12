#ifndef STM32WB55XX_SAI_H
#define STM32WB55XX_SAI_H

#include <stdint.h>
#include <stdbool.h>
#include <mcu/stm32wb55xx/gpio.h>
#include <libraries/error.h>

typedef uint32_t sai_interface_handle_t;
typedef uint32_t sai_device_handle_t;

typedef enum sai_mode {
    SAI_MODE_MASTER_TRANSMITTER,
    SAI_MODE_MASTER_RECEIVER,
    SAI_MODE_SLAVE_TRANSMITTER,
    SAI_MODE_SLAVE_RECEIVER,
} sai_mode_t;

typedef enum sai_protocol {
    SAI_PROTOCOL_FREE,
    SAI_PROTOCOL_SPDIF,
    SAI_PROTOCOL_AC97
} sai_protocol_t;

typedef enum sai_data_size {
    SAI_DATA_SIZE_RESERVED0,
    SAI_DATA_SIZE_RESERVED1,
    SAI_DATA_SIZE_8BITS,
    SAI_DATA_SIZE_10BITS,
    SAI_DATA_SIZE_16BITS,
    SAI_DATA_SIZE_20BITS,
    SAI_DATA_SIZE_24BITS,
    SAI_DATA_SIZE_32BITS,
} sai_data_size_t;

typedef enum sai_clock_strobe_edge {
    SAI_CLOCK_STROBE_EDGE_FALLING,
    SAI_CLOCK_STROBE_EDGE_RISING,
} sai_clock_strobe_edge_t;

typedef enum sai_frame_definition {
    SAI_FRAME_DEFINITION_COMBINED,
    SAI_FRAME_DEFINITION_SPLIT
} sai_frame_definition_t;

typedef enum sai_slot_size {
    SAI_SLOT_SIZE_DATA_SIZE,
    SAI_SLOT_SIZE_16BIT,
    SAI_SLOT_SIZE_32BIT,
    SAI_SLOT_SIZE_RESERVED
} sai_slot_size_t;

typedef struct sai_device_configuration {
    SAI_TypeDef *sai;
    SAI_Block_TypeDef *block;

    // Block config
    sai_mode_t mode;
    sai_protocol_t protocol;
    sai_data_size_t data_size;
    bool lsb_first;
    uint32_t frequency;
    sai_clock_strobe_edge_t clock_strobe_edge;

    // Frame config
    sai_frame_definition_t frame_definition;
    uint8_t frame_length;
    uint8_t sample_length;

    // Slot config
    uint8_t first_bit_offset;
    uint8_t slot_count;
    uint16_t slot_enable;
    sai_slot_size_t slot_size;
} sai_device_configuration_t;

error_t sai_interface_init(SAI_TypeDef *sai, uint32_t mclk_pin, uint32_t sclk_pin, uint32_t sd_pin, uint32_t fs_pin);
error_t sai_interface_deinit(sai_interface_handle_t handle);

error_t sai_device_init(sai_device_configuration_t config, sai_device_handle_t *handle);
error_t sai_device_deinit(sai_device_handle_t handle);

error_t sai_read(sai_device_handle_t handle, void *buffer, uint32_t length);
error_t sai_write(sai_device_handle_t handle, void *data, uint32_t length);

#endif
