#include <libraries/error.h>

static struct error_message g_error_message[] = {
    {SUCCESS, "Success"},
    {ERROR_INVALID, "Invalid argument"},
    {ERROR_NO_MEMORY, "Out of memory"},
    {ERROR_IO, "I/O Error"},
    {ERROR_TIMEOUT, "Timed out"},
    {ERROR_FAULT, "Fault"},
    {ERROR_NOT_FOUND, "Resource not found"},
    {ERROR_NOT_IMPLEMENTED, "Function not implemented"}
};

const char *error_get_message(error_t error)
{
    return g_error_message[error].message;
}
