#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <stdint.h>

void rcc_reset()
{
    RCC->CFGR = 0x00070000;
    RCC->CR &= 0xFAF6FEFB;
    RCC->CSR &= 0xFFFFFFFA;
    RCC->CRRCR &= 0xFFFFFFFE;
    RCC->PLLCFGR = 0x22041000;
    RCC->PLLSAI1CFGR = 0x22041000;
    RCC->CIER = 0x00000000;
    RCC->SMPSCR = 0x00000301;
    RCC->AHB1ENR = 0x00000000;
    RCC->AHB2ENR = 0x00000000;
    RCC->AHB3ENR = 0x02080000;
    RCC->APB1ENR1 = 0x00000400;
    RCC->APB1ENR2 = 0x00000000;
    RCC->APB2ENR = 0x00000000;
    RCC->AHB1SMENR = 0x00011207;
    RCC->AHB2SMENR = 0x0001209F;
    RCC->AHB3SMENR = 0x03070100;
    RCC->APB1SMENR1 = 0x85A04E01;
    RCC->APB1SMENR2 = 0x00000021;
    RCC->APB2SMENR = 0x00265800;
    RCC->CCIPR = 0x00000000;
}

void rcc_set_msi_clock_speed(uint32_t clock_speed)
{
    rcc_msi_clock_speed_t msi_clock_speed;

    switch (clock_speed) {
    case 100000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_100kHz;
        break;
    case 200000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_200kHz;
        break;
    case 400000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_400kHz;
        break;
    case 800000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_800kHz;
        break;
    case 1000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_1MHz;
        break;
    case 2000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_2MHz;
        break;
    case 4000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_4MHz;
        break;
    case 8000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_8MHz;
        break;
    case 16000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_16MHz;
        break;
    case 24000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_24MHz;
        break;
    case 32000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_32MHz;
        break;
    case 48000000:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_48MHz;
        break;
    default:
        msi_clock_speed = RCC_MSI_CLOCK_SPEED_4MHz;
    }

    RCC->CR &= ~(RCC_CR_MSIRANGE);
    RCC->CR |= msi_clock_speed << RCC_CR_MSIRANGE_Pos;
}

void rcc_set_pll_clock_speed(uint32_t pll_clock_speed)
{
    rcc_disable_pll_clock();
    switch (pll_clock_speed) {
    case 64000000:
        RCC->CR &= ~(RCC_CR_MSIRANGE);
        RCC->CR |= RCC_MSI_CLOCK_SPEED_16MHz << RCC_CR_MSIRANGE_Pos;
        RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;
        RCC->PLLCFGR |= ((8 << RCC_PLLCFGR_PLLN_Pos) |
                         (0 << RCC_PLLCFGR_PLLM_Pos) |
                         (1 << RCC_PLLCFGR_PLLR_Pos) |
                         (1 << RCC_PLLCFGR_PLLSRC_Pos) |
                         (1 << RCC_PLLCFGR_PLLREN_Pos));
        break;
    }
    rcc_enable_pll_clock();
}

void rcc_set_system_clock_source(rcc_system_clock_source_t system_clock_source)
{
    FLASH->ACR |= 2 << FLASH_ACR_LATENCY_Pos;
    RCC->CFGR &= ~(RCC_CFGR_SW);
    RCC->CFGR |= (system_clock_source << RCC_CFGR_SW_Pos);
}

void rcc_set_lpuart1_clock_source(rcc_lpuart_clock_source_t lpuart_clock_source)
{
    RCC->CCIPR &= ~(RCC_CCIPR_LPUART1SEL);
    RCC->CCIPR |= lpuart_clock_source << RCC_CCIPR_LPUART1SEL_Pos;
}

void rcc_set_rtc_clock_source(rcc_rtc_clock_source_t rcc_clock_source)
{
    RCC->BDCR &= ~(RCC_BDCR_RTCSEL);
    RCC->BDCR |= rcc_clock_source << RCC_BDCR_RTCSEL_Pos;
}

uint32_t g_msi_clock_speeds[] =
    {100000, 200000, 400000, 800000,
     1000000, 2000000, 4000000, 8000000,
     16000000, 24000000, 32000000, 48000000};

uint32_t rcc_get_system_clock_speed()
{
    uint32_t system_clock_speed = 0;
    if ((RCC->CFGR & RCC_CFGR_SW) == RCC_SYSTEM_CLOCK_SOURCE_MSI) {
        system_clock_speed = g_msi_clock_speeds[(RCC->CR & RCC_CR_MSIRANGE) >> RCC_CR_MSIRANGE_Pos];

    } else if ((RCC->CFGR & RCC_CFGR_SW) == RCC_SYSTEM_CLOCK_SOURCE_HSI16) {
        system_clock_speed = 16000000;

    } else if ((RCC->CFGR & RCC_CFGR_SW) == RCC_SYSTEM_CLOCK_SOURCE_HSE) {
        system_clock_speed = 32000000;

    } else if ((RCC->CFGR & RCC_CFGR_SW) == RCC_SYSTEM_CLOCK_SOURCE_PLL) {
        // TODO
        system_clock_speed = 0;
    }
    return system_clock_speed;
}

void rcc_enable_msi_clock()
{
    RCC->CR |= RCC_CR_MSION;
}

void rcc_disable_msi_clock()
{
    RCC->CR &= ~RCC_CR_MSION;
}

void rcc_enable_pll_clock()
{
    RCC->CR |= RCC_CR_PLLON;
}

void rcc_disable_pll_clock()
{
    RCC->CR &= ~RCC_CR_PLLON;
}

void rcc_enable_gpioa_clock()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
}

void rcc_enable_gpiob_clock()
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
}

void rcc_enable_lpuart1_clock()
{
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;
}

void rcc_enable_spi1_clock()
{
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
}

void rcc_enable_lsi1_clock()
{
    RCC->CSR |= RCC_CSR_LSI1ON;
    while (!(RCC->CSR & RCC_CSR_LSI1RDY));
}

void rcc_disable_lsi1_clock()
{
    RCC->CSR &= ~RCC_CSR_LSI1ON;
}

void rcc_enable_lsi2_clock()
{
    RCC->CSR |= RCC_CSR_LSI2ON;
    while (!(RCC->CSR & RCC_CSR_LSI2RDY));
}

void rcc_disable_lsi2_clock()
{
    RCC->CSR &= ~RCC_CSR_LSI2ON;
}

void rcc_enable_rtc_clock()
{
    RCC->BDCR |= RCC_BDCR_RTCEN;
}

void rcc_disable_rtc_clock()
{
    RCC->BDCR &= ~RCC_BDCR_RTCEN;
}

void rcc_enable_i2c1_clock()
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C1EN;
}

void rcc_disable_i2c1_clock()
{
    RCC->APB1ENR1 &= ~RCC_APB1ENR1_I2C1EN;
}

void rcc_enable_i2c3_clock()
{
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN;
}

void rcc_disable_i2c3_clock()
{
    RCC->APB1ENR1 &= ~RCC_APB1ENR1_I2C3EN;
}

void rcc_enable_dma1_clock()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
}

void rcc_disable_dma1_clock()
{
    RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA1EN;
}

void rcc_enable_dmamux1_clock()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMAMUX1EN;
}

void rcc_disable_dmamux1_clock()
{
    RCC->AHB1ENR &= ~RCC_AHB1ENR_DMAMUX1EN;
}

void rcc_disable_interrupts()
{
    RCC->CIER = 0x00000000;
}

void rcc_backup_domain_reset()
{
    RCC->BDCR |= RCC_BDCR_BDRST;
}

void rcc_backup_domain_reset_clear()
{
    RCC->BDCR &= ~RCC_BDCR_BDRST;
}
