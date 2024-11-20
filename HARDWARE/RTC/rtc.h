#ifndef RTC_H
#define RTC_H
#include <stm32f4xx.h>
#define RTC_CLK_SRC_IS_LSE 1
#define RTC_CLK_SRC_IS_LSI 0
void rtc_init(void);
#endif
