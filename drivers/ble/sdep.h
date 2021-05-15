#ifndef SDEP_H
#define SDEP_H

#define SDEP_PAYLOAD_MAX_SIZE 16

typedef enum sdep_message_type {
    SDEP_MESSAGE_TYPE_COMMAND = 0x10,
    SDEP_MESSAGE_TYPE_RESPONSE = 0x20,
    SDEP_MESSAGE_TYPE_ALERT = 0x40,
    SDEP_MESSAGE_TYPE_ERROR = 0x80,
} sdep_message_type_t;

typedef enum sdep_command {
    SDEP_COMMAND_INITIALIZE = 0xBEEF,
    SDEP_COMMAND_ATWRAPPER = 0x0A00,
    SDEP_COMMAND_UARTTX = 0x0A01,
    SDEP_COMMAND_UARTRX = 0x0A02,
} sdep_command_t;

typedef struct __attribute__((packed)) sdep_header {
    sdep_message_type_t message_type;
    sdep_command_t command;
    struct __attribute__((packed)) {
        uint8_t payload_size : 7;
        uint8_t more_data : 1;
    };
} sdep_header_t;

#endif
