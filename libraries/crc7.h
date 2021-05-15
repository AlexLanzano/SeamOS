#ifndef CRC7_H
#define CRC7_H

#include <stdint.h>

uint8_t crc7_calculate(uint8_t *msg, uint32_t length);

#endif
