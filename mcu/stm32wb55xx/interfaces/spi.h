#ifndef STM32WB55XX_SPI_H
#define STM32WB55XX_SPI_H

#include <libraries/error.h>
#include <mcu/interfaces/spi.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/interfaces/gpio.h>
#include <mcu/stm32wb55xx/interfaces/dma.h>

typedef enum spi_baud_rate_prescaler {
    SPI_BAUD_RATE_PRESCALER_2,
    SPI_BAUD_RATE_PRESCALER_4,
    SPI_BAUD_RATE_PRESCALER_8,
    SPI_BAUD_RATE_PRESCALER_16,
    SPI_BAUD_RATE_PRESCALER_32,
    SPI_BAUD_RATE_PRESCALER_64,
    SPI_BAUD_RATE_PRESCALER_128,
    SPI_BAUD_RATE_PRESCALER_256
} spi_baud_rate_prescaler_t;

error_t spi_enable(SPI_TypeDef *spi, uint32_t sck_pin, uint32_t miso_pin, uint32_t mosi_pin);
error_t spi_disable(uint32_t handle);

#endif
