#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#include "rtc.h"
#include "gpio.h"
#include <rtthread.h>

typedef void(*Rtc_Alarm_Cb)(void);

/* 设置中断回调函数 */
void Bsp_Rtc_SetIrqCb(Rtc_Alarm_Cb cb);

/* 同步rtc时间并开始1分钟一次的中断 */
void Bsp_Rtc_Sync(uint8_t u8Year, uint8_t u8Month, uint8_t u8Day, uint8_t u8DayOfWeek, uint8_t u8Hour, uint8_t u8Minute, uint8_t u8Second);

//RTC获取时间函数
en_result_t Bsp_Rtc_GetTime(stc_rtc_time_t* time);

#endif
