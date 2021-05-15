#include <stdint.h>
#include <stdarg.h>
#include <libraries/string.h>

#define UINT_STRING_MAX 16

char g_char_lut[] = "0123456789abcdef";

void *memset(void *dest, uint8_t value, uint32_t size)
{
    uint8_t *d = dest;

    for (uint32_t i = 0; i < size; i++) {
            d[i] = value;
    }

    return dest;
}

void *memcpy(void *dest, const void *src, uint32_t size)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    for (uint32_t i = 0; i < size; i++) {
        d[i] = s[i];
    }

    return dest;
}

void *memcpy_reverse(void *dest, const void *src, uint32_t size)
{
    uint8_t *d = dest;
    const uint8_t *s = src;
    uint32_t i, n;
    for (i = 0, n = size-1; i < size; i++, n--) {
        d[i] = s[n];
    }

    return dest;
}

int memcmp(const void *mem1, const void *mem2, uint32_t size)
{
    const uint8_t *m1 = mem1;
    const uint8_t *m2 = mem2;

    for (uint32_t i = 0; i < size;i++) {
        if (m1[i] > m2[i]) {
            return 1;
        }

        if (m1[i] < m2[i]) {
            return -1;
        }
    }
    return 0;
}

void *memmove(void *dest, const void *source, uint32_t size)
{
    uint8_t temp[size];
    memcpy(temp, source, size);
    memcpy(dest, temp, size);

    return dest;
}

string_t string(char *cstring)
{
    uint32_t cstring_size = string_cstring_size(cstring);
    string_t string =
        {.error = (cstring == 0) ? ERROR_INVALID : SUCCESS,
        .data = cstring,
        .size = cstring_size,
        .capacity = cstring_size};
    return string;
}

string_t string_init(char *data, uint32_t size, uint32_t capacity)
{
    string_t string =
        {.error = (data == 0) ? ERROR_INVALID : SUCCESS,
        .data = data,
        .size = size,
        .capacity = capacity};
    return string;
}

uint32_t string_size(string_t string)
{
    if (!string_is_valid(string)) {
        return 0;
    }
    return string.size;
}

uint32_t string_cstring_size(const char *cstring)
{
    uint32_t count = 0;
    while(cstring[count++]);
    return count;
}

uint32_t string_capacity(string_t string)
{
    if (!string_is_valid(string)) {
        return 0;
    }
    return string.capacity;
}

char *string_data(string_t string)
{
    if (!string_is_valid(string)) {
        return 0;
    }
    return string.data;
}

char string_at(string_t string, uint32_t index)
{
    if (index > string.size) {
        return 0;
    }

    return string.data[index];
}

string_t *string_clear(string_t *string)
{
    if (!string) {
        return string;
    }
    if (!string_is_valid(*string)) {
        string->error = ERROR_INVALID;
        return string;
    }

    memset(string->data, 0, string->capacity);
    string->size = 0;

    return string;
}

string_t *string_copy(string_t *dest, string_t source)
{
    if (!dest) {
        return dest;
    }
    if (!string_is_valid(*dest) || !string_is_valid(source) ||
        dest->capacity < source.size) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    memcpy(dest->data, source.data, source.size);
    dest->size = source.size;

    return dest;
}

string_t *string_copy_cstring(string_t *dest, const char *cstring, uint32_t cstring_size)
{
    if (!dest) {
        return dest;
    }
    if (!string_is_valid(*dest) ||
        dest->capacity < cstring_size) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    memcpy(dest->data, cstring, cstring_size);
    dest->size = cstring_size;

    return dest;
}

string_t *string_append(string_t *dest, char c)
{
    if (!dest) {
        return dest;
    }
    if (!string_is_valid(*dest) ||
        dest->size == dest->capacity) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    dest->data[dest->size] = c;
    dest->size++;

    return dest;
}

string_t *string_concatenate(string_t *dest, string_t source)
{
    if (!dest) {
        return dest;
    }
    if (!string_is_valid(*dest) ||
        dest->size + source.size > dest->capacity) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    memcpy(&dest->data[dest->size], source.data, source.size);
    dest->size += source.size;

    return dest;
}

string_t *string_concatenate_reverse(string_t *dest, string_t source)
{
    if (!dest) {
        return dest;
    }
    if (!string_is_valid(*dest) ||
        dest->size + source.size > dest->capacity) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    memcpy_reverse(&dest->data[dest->size], source.data, source.size);
    dest->size += source.size;

    return dest;
}

string_t *string_concatenate_cstring(string_t *dest, const char *cstring, uint32_t cstring_size)
{
    if (!dest) {
        return dest;
    }

    if (!string_is_valid(*dest) ||
        dest->size + cstring_size > dest->capacity) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    memcpy(&dest->data[dest->size], cstring, cstring_size);
    dest->size += cstring_size;

    return dest;
}

string_t *string_format(string_t *dest, string_t format, va_list ap)
{
    if (!dest) {
        return dest;
    }

    if (!string_is_valid(*dest) || !string_is_valid(format)) {
        dest->error = ERROR_INVALID;
        return dest;
    }

    uint32_t u;
    int32_t d;
    string_t s;
    int c;

    char zero_padding_string_data[UINT_STRING_MAX];
    string_t zero_padding_string = string_init(zero_padding_string_data, 0, UINT_STRING_MAX);
    uint32_t zero_padding = 0;

    uint32_t state = 0;
    for (uint32_t i = 0; i < format.size; i++) {
        char ch = format.data[i];
        if (state == 0) { // looking for specifier
            if (ch == '%') {
                state = 1;
                continue;
            }

            dest = string_append(dest, ch);
        }

        if (state == 1) { // Process specifier
            if (ch == '0') {
                state = 2;
                continue;
            }

            if (ch == 'u') {
                char uint_string_data[UINT_STRING_MAX];
                string_t uint_string = string_init(uint_string_data, 0, UINT_STRING_MAX);
                u = va_arg(ap, uint32_t);

                uint_string = string_uint_to_string(uint_string, u, zero_padding, 10);
                dest = string_concatenate(dest, uint_string);
                if (dest->error) {
                    break;
                }
            }

            if (ch == 'i' ||
                ch == 'd') {
                char int_string_data[UINT_STRING_MAX];
                string_t int_string = string_init(int_string_data, 0, UINT_STRING_MAX);
                d = va_arg(ap, int32_t);

                int_string = string_int_to_string(int_string, d, zero_padding, 10);
                dest = string_concatenate(dest, int_string);
                if (dest->error) {
                    break;
                }
            }

            if (ch == 'x') {
                char hex_string_data[UINT_STRING_MAX];
                string_t hex_string = string_init(hex_string_data, 0, UINT_STRING_MAX);
                u = va_arg(ap, uint32_t);

                hex_string = string_uint_to_string(hex_string, u, zero_padding, 16);
                dest = string_concatenate(dest, hex_string);
                if (dest->error) {
                    break;
                }
            }

            if (ch == 's') {
                s = va_arg(ap, string_t);
                dest = string_concatenate(dest, s);
                if (dest->error) {
                    break;
                }
            }

            if (ch == 'c') {
                c = va_arg(ap, int);
                dest = string_append(dest, c);
                if (dest->error) {
                    break;
                }
            }
            state = 0;
            zero_padding = 0;
            string_clear(&zero_padding_string);
        }

        if (state == 2) { // Process zero padding
            if (ch < 48 || ch > 57 || i == format.size) { // if not a number or last char in format
                break;
            }

            string_append(&zero_padding_string, ch);
            if (zero_padding_string.error) {
                break;
            }

            // Check to see if the next char in format isn't a number
            if (format.data[i+1] < 48 || format.data[i+1] > 57) {
                zero_padding = string_to_uint(zero_padding_string);
                state = 1;
            }
        }
    }

    return dest;
}

bool string_is_equal(string_t string1, string_t string2)
{
    if (!string_is_valid(string1) || !string_is_valid(string2) ||
        string1.size != string2.size) {
        return false;
    }

    return (memcmp(string1.data, string2.data, string1.size) == 0);
}

bool string_is_valid(string_t string)
{
    if (string.error || !string.data) {
        return false;
    }

    return true;
}

uint32_t string_to_uint(string_t string)
{
    if (!string_is_valid(string)) {
        return 0;
    }

    uint32_t value = 0;
    for (uint32_t i = 0; i < string.size; i++) {
        uint8_t digit = string.data[i] - '0';
        if (digit > 9) {
            break;
        }
        value = (value * 10) + digit;
    }

    return value;
}

string_t string_uint_to_string(string_t dest, uint32_t u, uint32_t zero_padding, uint32_t base)
{
    if (!string_is_valid(dest) ||
        dest.capacity < UINT_STRING_MAX ||
        zero_padding > UINT_STRING_MAX) {
        dest.error = ERROR_INVALID;
        return dest;
    }

    char digit_string_data[UINT_STRING_MAX];
    string_t digit_string = string_init(digit_string_data, 0, UINT_STRING_MAX);
    do {
        uint8_t digit = u % base;
        string_append(&digit_string, g_char_lut[digit]);
        if (digit_string.error) {
            dest.error = digit_string.error;
            return dest;
        }
        u -= digit;
        u /= base;
        if (zero_padding) {
            zero_padding--;
        }
    } while (u || zero_padding);

    if (base == 16) {
        string_copy_cstring(&dest, "0x", 2);
    }

    string_concatenate_reverse(&dest, digit_string);

    return dest;
}

string_t string_int_to_string(string_t dest, int32_t i, uint32_t zero_padding, uint32_t base)
{
    if (i >= 0) {
        return string_uint_to_string(dest, i, zero_padding, base);
    }

    if (!string_is_valid(dest) ||
        dest.capacity < UINT_STRING_MAX ||
        zero_padding > UINT_STRING_MAX) {
        dest.error = ERROR_INVALID;
        return dest;
    }

    string_append(&dest, '-');

    char digit_string_data[UINT_STRING_MAX];
    string_t digit_string = string_init(digit_string_data, 0, UINT_STRING_MAX);
    do {
        uint8_t digit = i % base;
        string_append(&digit_string, g_char_lut[digit]);
        if (digit_string.error) {
            dest.error = digit_string.error;
            return dest;
        }
        i -= digit;
        i /= base;
        if (zero_padding) {
            zero_padding--;
        }
    } while (i || zero_padding);

    string_concatenate_reverse(&dest, digit_string);

    return dest;
}
