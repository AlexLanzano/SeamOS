#ifndef ERROR_H
#define ERROR_H

typedef enum {
    SUCCESS,
    ERROR_INVALID,
    ERROR_NO_MEMORY,
    ERROR_IO,
    ERROR_TIMEOUT,
    ERROR_FAULT,
    ERROR_LOCKED,
    ERROR_NOT_FOUND,
    ERROR_NOT_IMPLEMENTED,
} error_t;

struct error_message {
    error_t error_id;
    const char *message;
};

const char *error_get_message(error_t error);

#endif
