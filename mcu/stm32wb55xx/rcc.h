#ifndef RCC_H
#define RCC_H

#include <stdint.h>

typedef enum rcc_msi_clock_speed {
    RCC_MSI_CLOCK_SPEED_100kHz,
    RCC_MSI_CLOCK_SPEED_200kHz,
    RCC_MSI_CLOCK_SPEED_400kHz,
    RCC_MSI_CLOCK_SPEED_800kHz,
    RCC_MSI_CLOCK_SPEED_1MHz,
    RCC_MSI_CLOCK_SPEED_2MHz,
    RCC_MSI_CLOCK_SPEED_4MHz,
    RCC_MSI_CLOCK_SPEED_8MHz,
    RCC_MSI_CLOCK_SPEED_16MHz,
    RCC_MSI_CLOCK_SPEED_24MHz,
    RCC_MSI_CLOCK_SPEED_32MHz,
    RCC_MSI_CLOCK_SPEED_48MHz
} rcc_msi_clock_speed_t;

typedef enum rcc_system_clock_source {
    RCC_SYSTEM_CLOCK_SOURCE_MSI,
    RCC_SYSTEM_CLOCK_SOURCE_HSI16,
    RCC_SYSTEM_CLOCK_SOURCE_HSE,
    RCC_SYSTEM_CLOCK_SOURCE_PLL
} rcc_system_clock_source_t;

typedef enum rcc_pll_clock_source {
    RCC_PLL_CLOCK_SOURCE_NO_CLOCK,
    RCC_PLL_CLOCK_SOURCE_MSI,
    RCC_PLL_CLOCK_SOURCE_HSI16,
    RCC_PLL_CLOCK_SOURCE_HSE
} rcc_pll_clock_source_t;

typedef enum rcc_lpuart_clock_source {
    RCC_LPUART_CLOCK_SOURCE_PCLK,
    RCC_LPUART_CLOCK_SOURCE_SYSCLK,
    RCC_LPUART_CLOCK_SOURCE_HSI16,
    RCC_LPUART_CLOCK_SOURCE_LSE,
} rcc_lpuart_clock_source_t;

typedef enum rcc_rtc_clock_source {
    RCC_RTC_CLOCK_SOURCE_NO_CLOCK,
    RCC_RTC_CLOCK_SOURCE_LSE,
    RCC_RTC_CLOCK_SOURCE_LSI,
    RCC_RTC_CLOCK_SOURCE_HSE
} rcc_rtc_clock_source_t;

void rcc_reset();

void rcc_set_msi_clock_speed(uint32_t msi_clock_speed);
void rcc_set_pll_clock_speed(uint32_t pll_clock_speed);

void rcc_set_system_clock_source(rcc_system_clock_source_t system_clock_source);
void rcc_set_lpuart1_clock_source(rcc_lpuart_clock_source_t lpuart_clock_source);
void rcc_set_rtc_clock_source(rcc_rtc_clock_source_t rcc_clock_source);

uint32_t rcc_get_system_clock_speed();

void rcc_enable_msi_clock();
void rcc_disable_msi_clock();

void rcc_enable_pll_clock();
void rcc_disable_pll_clock();

void rcc_enable_gpioa_clock();
void rcc_disable_gpioa_clock();

void rcc_enable_gpiob_clock();
void rcc_disable_gpiob_clock();

void rcc_enable_lpuart1_clock();
void rcc_disable_lpuart1_clock();

void rcc_enable_spi1_clock();
void rcc_disable_spi1_clock();

void rcc_enable_lsi1_clock();
void rcc_disable_lsi1_clock();

void rcc_enable_lsi2_clock();
void rcc_disable_lsi2_clock();

void rcc_enable_rtc_clock();
void rcc_disable_rtc_clock();

void rcc_enable_i2c1_clock();
void rcc_disable_i2c1_clock();

void rcc_enable_i2c3_clock();
void rcc_disable_i2c3_clock();

void rcc_disable_interrupts();

void rcc_backup_domain_reset();
void rcc_backup_domain_reset_clear();
#endif
