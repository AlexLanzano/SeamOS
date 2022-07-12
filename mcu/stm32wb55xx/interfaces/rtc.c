#include <libraries/time.h>
#include <kernel/debug/log.h>
#include <libraries/error.h>
#include <mcu/stm32wb55xx/stm32wb55xx.h>
#include <mcu/stm32wb55xx/rcc.h>
#include <mcu/stm32wb55xx/rtc.h>

error_t rtc_set_date_and_time(datetime_t *datetime)
{
    if (!datetime) {
        log_error(ERROR_INVALID, "datetime is a pointer to a NULL address");
        return ERROR_INVALID;
    }

    // Disable backup domain write protection
    PWR->CR1 |= PWR_CR1_DBP;

    // Disable RTC register write protection
    RTC->WPR = 0xca;
    RTC->WPR = 0x53;

    // Change to initialzation mode
    RTC->ISR |= RTC_ISR_INIT;

    // Wait till we're in init mode
    while (!(RTC->ISR & RTC_ISR_INITF));

    // Set Prescalar value
    RTC->PRER &= ~RTC_PRER_PREDIV_S;
    RTC->PRER |= 0xF9 << RTC_PRER_PREDIV_S_Pos;

    // Convert to BCD format
    // TODO: maybe convert these to macros?
    uint8_t year_units = datetime->year % 10;
    uint8_t year_tens = (datetime->year / 10) % 10;

    uint8_t month_units = datetime->month % 10;
    uint8_t month_tens = (datetime->month / 10 ) % 10;

    uint8_t day_units = datetime->day % 10;
    uint8_t day_tens = (datetime->day / 10) % 10;

    uint8_t hours_units = datetime->hours % 10;
    uint8_t hours_tens = (datetime->hours / 10) % 10;

    uint8_t minutes_units = datetime->minutes % 10;
    uint8_t minutes_tens = (datetime->minutes / 10) % 10;

    uint8_t seconds_units = datetime->seconds % 10;
    uint8_t seconds_tens = (datetime->seconds / 10) % 10;

    // Set the date
    RTC->DR = ((year_tens << RTC_DR_YT_Pos) & RTC_DR_YT) | ((year_units << RTC_DR_YU_Pos) & RTC_DR_YU) |
              ((month_tens << RTC_DR_MT_Pos) & RTC_DR_MT) | ((month_units << RTC_DR_MU_Pos) & RTC_DR_MU) |
              ((day_tens << RTC_DR_DT_Pos) & RTC_DR_DT) | ((day_units << RTC_DR_DU_Pos) & RTC_DR_DU) |
              ((datetime->weekday << RTC_DR_WDU_Pos) & RTC_DR_WDU);

    // Set the time
    RTC->TR = ((hours_tens << RTC_TR_HT_Pos) & RTC_TR_HT) | ((hours_units << RTC_TR_HU_Pos) & RTC_TR_HU) |
              ((minutes_tens << RTC_TR_MNT_Pos) & RTC_TR_MNT) | ((minutes_units << RTC_TR_MNU_Pos) & RTC_TR_MNU) |
              ((seconds_tens << RTC_TR_ST_Pos) & RTC_TR_ST) | ((seconds_units << RTC_TR_SU_Pos) & RTC_TR_SU);

    // Exit init mode
    RTC->ISR &= ~RTC_ISR_INIT;

    // Enable RTC write protection
    RTC->WPR = 0x00;

    // Enable backup domain write protection
    PWR->CR1 &= ~PWR_CR1_DBP;

    return SUCCESS;
}

error_t rtc_get_date_and_time(datetime_t *datetime)
{
    if (!datetime) {
        log_error(ERROR_INVALID, "datetime is a pointer to a NULL address");
        return ERROR_INVALID;
    }
    datetime->year = (((RTC->DR & RTC_DR_YT) >> RTC_DR_YT_Pos) * 10) + ((RTC->DR & RTC_DR_YU) >> RTC_DR_YU_Pos);
    datetime->month = (((RTC->DR & RTC_DR_MT) >> RTC_DR_MT_Pos) * 10) + ((RTC->DR & RTC_DR_MU) >> RTC_DR_MU_Pos);
    datetime->day = (((RTC->DR & RTC_DR_DT) >> RTC_DR_DT_Pos) * 10) + ((RTC->DR & RTC_DR_DU) >> RTC_DR_DU_Pos);
    datetime->weekday = (RTC->DR & RTC_DR_WDU) >> RTC_DR_WDU_Pos;

    datetime->hours = (((RTC->TR & RTC_TR_HT) >> RTC_TR_HT_Pos) * 10) + ((RTC->TR & RTC_TR_HU) >> RTC_TR_HU_Pos);
    datetime->minutes = (((RTC->TR & RTC_TR_MNT) >> RTC_TR_MNT_Pos) * 10) +
                        ((RTC->TR & RTC_TR_MNU) >> RTC_TR_MNU_Pos);
    datetime->seconds = (((RTC->TR & RTC_TR_ST) >> RTC_TR_ST_Pos) * 10) + ((RTC->TR & RTC_TR_SU) >> RTC_TR_SU_Pos);

    return SUCCESS;
}

error_t rtc_init()
{
    datetime_t datetime;

    rcc_enable_lsi1_clock();

    // Disable backup domain write protection
    PWR->CR1 |= PWR_CR1_DBP;
    rcc_backup_domain_reset();
    rcc_backup_domain_reset_clear();

    rcc_set_rtc_clock_source(RCC_RTC_CLOCK_SOURCE_LSI);
    rcc_enable_rtc_clock();

    datetime.year = 70;
    datetime.month = 0;
    datetime.day = 1;
    datetime.weekday = 0;

    datetime.hours = 0;
    datetime.minutes = 0;
    datetime.seconds = 0;

    return rtc_set_date_and_time(&datetime);
}
