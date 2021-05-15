#include <stdint.h>
#include <drivers/ble/bluefruit.h>
#include <mcu/spi.h>
#include <mcu/gpio.h>
#include <mcu/system_timer.h>
#include <kernel/debug/log.h>
#include <libraries/string.h>
#include <libraries/error.h>
#include "sdep.h"



static gpio_handle_t g_cs_pin_handle;
static gpio_handle_t g_irq_pin_handle;
static spi_handle_t g_spi_handle;

static inline void bluefruit_select()
{
    gpio_write(g_cs_pin_handle, 0);
}


static inline void bluefruit_deselect()
{
    gpio_write(g_cs_pin_handle, 1);
}

static error_t bluefruit_send(sdep_command_t command, void *payload, uint32_t size)
{
    sdep_header_t packet;
    error_t error = SUCCESS;

    packet.message_type = SDEP_MESSAGE_TYPE_COMMAND;
    packet.command = command;
    packet.payload_size = size;
    log_debug("sending packet:\r\n type: %x\r\n command: %x\r\n length: %u\r\n",
              packet.message_type, packet.command, size);

    bluefruit_select();
    error = spi_write(g_spi_handle, &packet, sizeof(sdep_header_t));
    if (error) {
        goto exit;
    }

    if (size && payload) {
        error = spi_write(g_spi_handle, payload, size);
        if (error) {
            goto exit;
        }
    }

 exit:
    bluefruit_deselect();
    return error;
}

static error_t bluefruit_receive(sdep_header_t *packet_header, void *payload)
{
    if (!packet_header || !payload) {
        log_error(ERROR_INVALID, "NULL header or payload");
        return ERROR_INVALID;
    }

    uint8_t irq_value;
    error_t error = SUCCESS;
    uint32_t count = 0;

    // Check if IRQ pin is raised and a packet is ready to be received
    do {
        gpio_read(g_irq_pin_handle, &irq_value);
        count++;
    } while(irq_value == 0 && count < 100);

    if (count == 100) {
        log_error(ERROR_IO, "No data to be received");
        return ERROR_IO;
    }

    bluefruit_select();

    count = 0;

    do {
        // Find the packet header
        error = spi_read_write(g_spi_handle, &packet_header->message_type, 0xff, 1);
        if (error) {
            log_error(error, "Failed to get packet header");
            goto exit;
        }

        if (packet_header->message_type == 0xff) {
            bluefruit_deselect();
            system_timer_wait_ms(50);
            bluefruit_select();
        }
    } while (packet_header->message_type != SDEP_MESSAGE_TYPE_RESPONSE &&
             packet_header->message_type != SDEP_MESSAGE_TYPE_ERROR &&
             count++ < 50);

    if (count == 50) {
        error = ERROR_IO;
        log_error(error, "Failed to read packet type");
        goto exit;
    }

    // Receive the rest of the packet header
    error = spi_read_write(g_spi_handle, (&packet_header->message_type)+1, 0xff, 3);
    if (error) {
        log_error(error, "Failed to read packet header");
        goto exit;
    }

    if (packet_header->payload_size > 16) {
        error = ERROR_INVALID;
        log_error(error, "payload size too big");
        goto exit;
    }

    log_debug("\r\nRecv packet:\r\n type: %x\r\n cmd: %x\r\n size: %u\r\n",
              packet_header->message_type,packet_header->command,packet_header->payload_size);

    // Receive the payload
    error = spi_read_write(g_spi_handle, payload, 0xff, packet_header->payload_size);
    if (error) {
        log_error(error, "Failed to receieve payload");
    }

 exit:
    bluefruit_deselect();
    return error;
}

error_t bluefruit_read(void *buffer, uint32_t size)
{
    if (!buffer) {
        return ERROR_INVALID;
    }

    error_t error;
    uint8_t *buffer_ptr = buffer;
    sdep_header_t packet_header;
    uint8_t payload[16];
    memset(&packet_header, 0, sizeof(sdep_header_t));

    do {
        error = bluefruit_receive(&packet_header, payload);
        if (error) {
            log_error(error, "Failed to receive packet");
            return error;
        }
        uint32_t copy_size = (size < packet_header.payload_size) ? size : packet_header.payload_size;
        size -= copy_size;
        memcpy(buffer_ptr, payload, copy_size);
        buffer_ptr += copy_size;
        // Need to put in this wait, looks like packets get corrupted if not. Maybe clock too fast?
        system_timer_wait_ms(50);
    } while (packet_header.more_data);

    return error;
}

error_t bluefruit_write(void *buffer, uint32_t size)
{
    return bluefruit_send(SDEP_COMMAND_ATWRAPPER, buffer, size);
}

error_t bluefruit_init(bluefruit_configuration_t config)
{
    error_t error;

    g_cs_pin_handle = config.cs_pin_handle;
    g_irq_pin_handle = config.irq_pin_handle;
    g_spi_handle = config.spi_handle;

    // Reset Bluefruit Device
    error = bluefruit_send(SDEP_COMMAND_INITIALIZE, 0, 0);
    if (error) {
        log_error(error, "Failed to send rest command");
        return error;
    }
    // Wait one second to reboot Bluefruit
    system_timer_wait_ms(1000);

    return SUCCESS;
}
