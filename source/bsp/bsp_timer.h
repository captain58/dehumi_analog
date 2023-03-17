#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#include "ddl.h"
#include "timer3.h"
#include <rtthread.h>
#include "bsp/bsp_gpio.h"
uint32_t Bsp_Timer3_GetMsCnt(void);
uint32_t Bsp_Timer3_GetUsCnt(void);
uint32_t Bsp_Timer_Sub(uint32_t time1, uint32_t time2);
int Bsp_Timer_Init(void);

#define BSP_PWM_GPIO            (GpioPortB)
#define BSP_PWM_PIN             (GpioPin3)
#define BSP_PWM_FENGSHAN            (0)

/* 加湿泵 */
#define   WATER_PUMP_GPIO           (GpioPortB)
#define   WATER_PUMP_PIN            (GpioPin2)
#define BSP_PWM_WATERPUMP            (1)

void Bsp_Pwm_SetStatus(uint8_t uiIndex, uint8_t uiFlag);
/* 设置pwm开关 0 关 1 低 2 中 3 高 */
void Bsp_Timer_PwmEnable(uint8_t uiIndex, uint8_t uiLevel);
/* 设置pwm高低电平延时时间，单位ms */
void Bsp_Timer_Pwm_SetPara(uint8_t uiIndex, uint16_t uiHighTime, uint16_t uiLowTime);
#endif
