#include <stm32wb55xx.h>
#include <stm32wb55xx/rcc.h>
#include <stdint.h>
#include <libraries/string.h>

#define VECT_TAB_OFFSET 0x0

extern uint32_t _estack[];

extern void main();

void __attribute__((naked,noreturn)) Default_Handler()
{
    while(1);
}

void Reset_Handler() __attribute__((weak));
void NMI_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HardFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void MemManage_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void BusFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void UsageFault_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SVC_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DebugMon_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PendSV_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SysTick_Handler() __attribute__((weak, noreturn, alias("Default_Handler")));
void WWDG_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PVD_PVM_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TAMP_STAMP_LSECSS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RTC_WKUP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FLASH_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RCC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI0_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA1_Channel7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void ADC1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USB_HP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USB_LP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void C2SEV_PWR_C2H_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void COMP_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI9_5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_BRK_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_UP_TIM16_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_TRG_COM_TIM17_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM1_CC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TIM2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PKA_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C1_EV_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C1_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C3_EV_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void I2C3_ER_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SPI2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void USART1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPUART1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void SAI1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void TSC_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void EXTI15_10_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RTC_Alarm_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void CRS_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void IPCC_C1_RX_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void IPCC_C1_TX_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void HSEM_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LPTIM2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void LCD_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void QUADSPI_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void AES1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void AES2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void RNG_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void FPU_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel1_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel2_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel3_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel4_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel5_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel6_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMA2_Channel7_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));
void DMAMUX1_OVR_IRQHandler() __attribute__((weak, noreturn, alias("Default_Handler")));

#define RESERVED Default_Handler

void (* const interrupt_vector_table[])() __attribute__((section(".isr_vector"))) = {
    (void (*)())_estack,
    Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManage_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    RESERVED,
    RESERVED,
    RESERVED,
    RESERVED,
    SVC_Handler,
    DebugMon_Handler,
    RESERVED,
    PendSV_Handler,
    SysTick_Handler,
    WWDG_IRQHandler,
    PVD_PVM_IRQHandler,
    TAMP_STAMP_LSECSS_IRQHandler,
    RTC_WKUP_IRQHandler,
    FLASH_IRQHandler,
    RCC_IRQHandler,
    EXTI0_IRQHandler,
    EXTI1_IRQHandler,
    EXTI2_IRQHandler,
    EXTI3_IRQHandler,
    EXTI4_IRQHandler,
    DMA1_Channel1_IRQHandler,
    DMA1_Channel2_IRQHandler,
    DMA1_Channel3_IRQHandler,
    DMA1_Channel4_IRQHandler,
    DMA1_Channel5_IRQHandler,
    DMA1_Channel6_IRQHandler,
    DMA1_Channel7_IRQHandler,
    ADC1_IRQHandler,
    USB_HP_IRQHandler,
    USB_LP_IRQHandler,
    C2SEV_PWR_C2H_IRQHandler,
    COMP_IRQHandler,
    EXTI9_5_IRQHandler,
    TIM1_BRK_IRQHandler,
    TIM1_UP_TIM16_IRQHandler,
    TIM1_TRG_COM_TIM17_IRQHandler,
    TIM1_CC_IRQHandler,
    TIM2_IRQHandler,
    PKA_IRQHandler,
    I2C1_EV_IRQHandler,
    I2C1_ER_IRQHandler,
    I2C3_EV_IRQHandler,
    I2C3_ER_IRQHandler,
    SPI1_IRQHandler,
    SPI2_IRQHandler,
    USART1_IRQHandler,
    LPUART1_IRQHandler,
    SAI1_IRQHandler,
    TSC_IRQHandler,
    EXTI15_10_IRQHandler,
    RTC_Alarm_IRQHandler,
    CRS_IRQHandler,
    PWR_SOTF_BLEACT_802ACT_RFPHASE_IRQHandler,
    IPCC_C1_RX_IRQHandler,
    IPCC_C1_TX_IRQHandler,
    HSEM_IRQHandler,
    LPTIM1_IRQHandler,
    LPTIM2_IRQHandler,
    LCD_IRQHandler,
    QUADSPI_IRQHandler,
    AES1_IRQHandler,
    AES2_IRQHandler,
    RNG_IRQHandler,
    FPU_IRQHandler,
    DMA2_Channel1_IRQHandler,
    DMA2_Channel2_IRQHandler,
    DMA2_Channel3_IRQHandler,
    DMA2_Channel4_IRQHandler,
    DMA2_Channel5_IRQHandler,
    DMA2_Channel6_IRQHandler,
    DMA2_Channel7_IRQHandler,
    DMAMUX1_OVR_IRQHandler,
};
