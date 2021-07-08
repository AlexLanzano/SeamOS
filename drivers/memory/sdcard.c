#include <drivers/memory/sdcard.h>
#include <mcu/spi.h>
#include <mcu/gpio.h>
#include <mcu/system_timer.h>
#include <kernel/debug/log.h>
#include <libraries/crc7.h>
#include <libraries/string.h>
#include <libraries/error.h>

static spi_device_handle_t g_spi_handle;

static error_t sdcard_get_response(void *response, uint32_t length)
{
    return spi_read_write(g_spi_handle, response, 0xff, length);
}

static error_t sdcard_wait_till_ready()
{
    uint8_t timeout = 0;
    uint8_t resp = 0;
    while (resp != 0xff && timeout < 50) {
        error_t error = sdcard_get_response(&resp, 1);
        if (error) {
            log_error(error, "Failed to get response");
            return error;
        }
        timeout++;
    }

    if (timeout == 50) {
        log_error(ERROR_TIMEOUT, "Timed out while trying to get response");
        return ERROR_TIMEOUT;
    }

    return SUCCESS;
}

static error_t sdcard_send_command(uint8_t cmd, uint32_t arg, uint8_t *response)
{
    error_t error;
    uint8_t packet[6];
    uint8_t msg[] = {
        cmd | 0x40,
        (uint8_t)(arg >> 24),
        (uint8_t)(arg >> 16),
        (uint8_t)(arg >> 8),
        (uint8_t)(arg)
    };
    uint8_t crc = crc7_calculate(msg, 5) + 1;

    memcpy(packet, msg, 5);
    memcpy(&packet[5], &crc, 1);

    error = sdcard_wait_till_ready();
    if (error) {
        log_error(error, "SDcard not ready");
        return error;
    }

    error = spi_write(g_spi_handle, packet, 6);
    if (error) {
        log_error(error, "Failed to send packet");
        return error;
    }

    uint8_t count = 10;
    do {
        error_t error = sdcard_get_response(response, 1);
        if (error) {
            log_error(error, "Failed to acknowledge sent packet");
            return error;
        }
        count--;
    } while ((*response & 0x80) && count);

    if (!count) {
        log_error(ERROR_TIMEOUT, "Timed out while trying to get packet acknowledgment");
        return ERROR_TIMEOUT;
    }
    return SUCCESS;
}

error_t sdcard_read_block(uint32_t addr, void *buffer)
{
    error_t error;
    uint8_t response;
    uint8_t *data = buffer;
    uint16_t crc;

    spi_device_enable(g_spi_handle);

    do {
        error_t error = sdcard_send_command(17, addr, &response);
        if (error) {
            log_error(error, "Failed to send READ_SINGLE_BLOCK command");
            goto exit;
        }
    } while (response);

    // Wait for data block
    uint8_t temp = 0xff;
    while (temp == 0xff) {
        error = spi_read_write(g_spi_handle, &temp, 0xff, 1);
        if (error) {
            log_error(error, "Failed to wait for data block to be sent");
            goto exit;
        }
    }

    // Get data block
    error = sdcard_get_response(data, 512);
    if (error) {
        log_error(error, "Failed to get data");
        goto exit;
    }

    error = sdcard_get_response(&crc, 2);
    if (error) {
        log_error(error, "Failed to get CRC");
    }

 exit:
    spi_device_disable(g_spi_handle);
    return error;
}

error_t sdcard_write_block(uint32_t addr, void *buffer)
{
    error_t error;
    uint8_t response;

    spi_device_enable(g_spi_handle);
    do {
        error = sdcard_send_command(24, addr, &response);
        if (error) {
            log_error(error, "Failed to send WRITE_BLOCK command");
            goto exit;
        }
    } while (response);

    uint8_t msg[515];
    uint8_t data_start_token = 0xfe;
    uint16_t crc = 0xffff;
    uint8_t resp;

    memcpy(&msg[0], &data_start_token, 1);
    memcpy(&msg[1], buffer, 512);
    memcpy(&msg[513], &crc, 2);

    error = spi_write(g_spi_handle, msg, 515);
    if (error) {
        log_error(error, "Failed to write data");
        goto exit;
    }

    while ((resp & 0x1f) != 0x5) {
        error = sdcard_get_response(&resp, 1);
        if (error) {
            log_error(error, "Failed to get response");
            goto exit;
        }
    }

    error = sdcard_send_command(13, 0, &response);
    if (error) {
        log_error(error, "Failed to send SEND_STATUS command");
        goto exit;
    }
    error = sdcard_get_response(&resp, 1);
    if (error) {
        log_error(error, "Failed to get response from SEND_STATUS command");
    }

 exit:
    spi_device_disable(g_spi_handle);
    return error;
}

error_t sdcard_init(sdcard_configuration_t config)
{
    error_t error;

    g_spi_handle = config.spi_handle;

    error = spi_device_disable(g_spi_handle);
    if (error) {
        log_error(error, "Failed to set CS pin");
        return error;
    }

    for (uint32_t i = 0; i < 100; i++) {
        uint8_t msg = 0xff;
        error = spi_write(g_spi_handle, &msg, 1);
        if (error) {
            log_error(error, "Failed to send setup bits");
            return error;
        }
    }

    spi_device_enable(g_spi_handle);

    uint8_t timeout = 100;
    uint8_t response;
    do {
        error = sdcard_send_command(0, 0, &response);
        if (error) {
            log_error(error, "Failed to send GO_IDLE_STATE command");
            goto exit;
        }
        timeout--;
    } while (response != 0x01 && timeout);

    // Check if command timed out
    if (timeout == 0) {
        error = ERROR_TIMEOUT;
        log_error(error, "Failed to get response from GO_IDLE_STATE command");
        goto exit;
    }

    error = sdcard_send_command(8, 0x1AA, &response);
    if (error) {
        log_error(error, "Failed to send GET_IF_COND command");
        goto exit;
    }

    if (response & 0x4) {
        error = ERROR_IO;
        log_error(error, "Failed to get response from GET_IF_COND command");
        goto exit;
    }

    timeout = 200;
    do {
        error = sdcard_send_command(55, 0, &response); // TODO: Check response value here?
        if (error) {
            log_error(error, "Failed to send APP_CMD command");
            goto exit;
        }

        error = sdcard_send_command(41, 0x40000000, &response);
        if (error) {
            log_error(error, "Failed to send SD_SEND_OP_COND command");
            goto exit;
        }

        timeout--;
    } while (response != 0 && timeout);
    if (timeout == 0) {
        error = ERROR_TIMEOUT;
        log_error(error, "Timed out while trying to send SD_SEND_OP_COND command");
    }
 exit:
    spi_device_disable(g_spi_handle);
    return error;
}
