#include <config.h>
#include <stdint.h>
#include <stdbool.h>
#include <mcu/stm32wb55xx/sai.h>
#include <libraries/error.h>
#include <libraries/string.h>
#include <kernel/debug/log.h>

#ifndef CONFIG_SAI_DEVICE_MAX
#define CONFIG_SAI_DEVICE_MAX 1
#endif

typedef struct sai_device {
    bool is_initialized;
    sai_device_configuration_t config;
} sai_device_t;

sai_device_t g_sai_devices[CONFIG_SAI_DEVICE_MAX];

static bool sai_invalid_handle(const sai_device_handle_t handle)
{
    return (handle >= CONFIG_SAI_DEVICE_MAX || !g_sai_devices[handle].is_initialized);
}

static error_t sai_find_free_device(sai_device_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    sai_device_handle_t sai_handle;
    for (sai_handle = 0; sai_handle < CONFIG_SAI_DEVICE_MAX; sai_handle++) {
        if (!g_sai_devices[sai_handle].is_initialized) {
            *handle = sai_handle;
            return SUCCESS;
        }
    }

    return ERROR_NO_MEMORY;
}

static error_t sai_configure(const sai_device_handle_t handle)
{
    if (sai_invalid_handle(handle)) {
        return ERROR_INVALID;
    }

    sai_device_configuration_t *config = &g_sai_devices[handle].config;
    uint32_t mckdiv =  (CONFIG_CLOCK_FREQ) / (config->frequency * config->frame_length);

    config->block->CR1 = ((config->mode << SAI_xCR1_MODE_Pos) |
                          (config->protocol << SAI_xCR1_PRTCFG_Pos) |
                          (config->data_size << SAI_xCR1_DS_Pos) |
                          (config->lsb_first << SAI_xCR1_LSBFIRST_Pos) |
                          (config->clock_strobe_edge << SAI_xCR1_CKSTR_Pos) |
                          (0 << SAI_xCR1_SYNCEN_Pos) |
                          (0 << SAI_xCR1_MONO_Pos) |
                          (0 << SAI_xCR1_OUTDRIV_Pos) |
                          (0 << SAI_xCR1_SAIEN_Pos) |
                          (0 << SAI_xCR1_DMAEN_Pos) |
                          (0 << SAI_xCR1_NODIV_Pos) |
                          (mckdiv << SAI_xCR1_MCKDIV_Pos) |
                          (0 << SAI_xCR1_OSR_Pos) |
                          (0 << SAI_xCR1_MCKEN_Pos));

    config->block->CR2 = ((0 << SAI_xCR2_FTH_Pos) |
                          (0 << SAI_xCR2_FFLUSH_Pos) |
                          (0 << SAI_xCR2_TRIS_Pos) |
                          (0 << SAI_xCR2_MUTE_Pos) |
                          (0 << SAI_xCR2_MUTEVAL_Pos) |
                          (0 << SAI_xCR2_MUTECNT_Pos) |
                          (0 << SAI_xCR2_CPL_Pos) |
                          (0 << SAI_xCR2_COMP_Pos));

    config->block->FRCR = (((config->frame_length - 1) << SAI_xFRCR_FRL_Pos) |
                           ((config->sample_length - 1) << SAI_xFRCR_FSALL_Pos) |
                           (config->frame_definition << SAI_xFRCR_FSDEF_Pos) |
                           (0 << SAI_xFRCR_FSPOL_Pos) |
                           (1 << SAI_xFRCR_FSOFF_Pos));

    config->block->SLOTR = ((0 << SAI_xSLOTR_FBOFF_Pos) |
                            (config->slot_size << SAI_xSLOTR_SLOTSZ_Pos) |
                            ((config->slot_count - 1) << SAI_xSLOTR_NBSLOT_Pos) |
                            (config->slot_enable << SAI_xSLOTR_SLOTEN_Pos));

    config->block->CR1 |= 1 << SAI_xCR1_SAIEN_Pos;

    return SUCCESS;
}

error_t sai_interface_init(SAI_TypeDef *sai, uint32_t mclk_pin, uint32_t sck_pin, uint32_t sd_pin, uint32_t fs_pin)
{
    error_t error;

    error = gpio_init((gpio_configuration_t)
                      {.mode = GPIO_MODE_ALT_FUNC,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_LOW,
                       .pull_resistor = GPIO_PULL_RESISTOR_NONE,
                       .alternative_function = 13},
                      mclk_pin);
    if (error) {
        return error;
    }

    error = gpio_init((gpio_configuration_t)
                      {.mode = GPIO_MODE_ALT_FUNC,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_LOW,
                       .pull_resistor = GPIO_PULL_RESISTOR_NONE,
                       .alternative_function = 13},
                      sck_pin);
    if (error) {
        return error;
    }

    error = gpio_init((gpio_configuration_t)
                      {.mode = GPIO_MODE_ALT_FUNC,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_LOW,
                       .pull_resistor = GPIO_PULL_RESISTOR_NONE,
                       .alternative_function = 13},
                      sd_pin);
    if (error) {
        return error;
    }

    error = gpio_init((gpio_configuration_t)
                      {.mode = GPIO_MODE_ALT_FUNC,
                       .output_type = GPIO_OUTPUT_TYPE_PUSH_PULL,
                       .output_speed = GPIO_OUTPUT_SPEED_LOW,
                       .pull_resistor = GPIO_PULL_RESISTOR_NONE,
                       .alternative_function = 13},
                      fs_pin);
    if (error) {
        return error;
    }

    return SUCCESS;
}

error_t sai_interface_deinit(sai_interface_handle_t handle)
{
    // TODO: Implement
    return SUCCESS;
}

error_t sai_device_init(sai_device_configuration_t config, sai_device_handle_t *handle)
{
    if (!handle) {
        return ERROR_INVALID;
    }

    error_t error;

    error = sai_find_free_device(handle);
    if (error) {
        return error;
    }

    memcpy(&g_sai_devices[*handle].config, &config, sizeof(sai_device_configuration_t));
    g_sai_devices[*handle].is_initialized = true;
    sai_configure(*handle);

    return SUCCESS;
}

error_t sai_device_deinit(sai_device_handle_t handle)
{
    // TODO: Implement
    return SUCCESS;

}

error_t sai_read(sai_device_handle_t handle, void *buffer, uint32_t length)
{
    if (sai_invalid_handle(handle) || !buffer) {
        return ERROR_INVALID;
    }

    sai_device_configuration_t *config = &g_sai_devices[handle].config;
    uint8_t *buffer_ptr = buffer;

    while (length) {
        if (config->data_size == SAI_DATA_SIZE_8BITS) {
            uint8_t data = config->block->DR;

            *buffer_ptr = data;
            buffer_ptr++;
        } else if (config->data_size <= SAI_DATA_SIZE_16BITS) {
            uint16_t data = config->block->DR;

            *buffer_ptr = (uint8_t)data;
            buffer_ptr++;
            *buffer_ptr = (uint8_t)(data >> 8);
            buffer_ptr++;
        } else {
            uint32_t data = config->block->DR;

            *buffer_ptr = (uint8_t)data;
            buffer_ptr++;
            *buffer_ptr = (uint8_t)(data >> 8);
            buffer_ptr++;
            *buffer_ptr = (uint8_t)(data >> 16);
            buffer_ptr++;
            *buffer_ptr = (uint8_t)(data >> 24);
            buffer_ptr++;
        }
        length--;
    }

    // Clear the status flags
    config->block->CLRFR |= 0x77;

    return SUCCESS;
}

error_t sai_write(sai_device_handle_t handle, void *data, uint32_t length)
{
    return SUCCESS;
}
