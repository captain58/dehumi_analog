#ifndef __BSP_ADC_NTC_H__
#define __BSP_ADC_NTC_H__

#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include <rtthread.h>

/* 管盘温度adc */
#define BSP_ADC_NTC_GPIO            (GpioPortA)
#define BSP_ADC_NTC_PIN             (GpioPin0)
#define BSP_ADC_NTC_CHANAL          (AdcExInputCH0)

/* 温度adc */
#define BSP_ADC_TEMP_GPIO            (GpioPortB)
#define BSP_ADC_TEMP_PIN             (GpioPin10)
#define BSP_ADC_TEMP_CHANAL          (AdcExInputCH17)

/* 湿度adc */
#define BSP_ADC_HUMP_GPIO            (GpioPortB)
#define BSP_ADC_HUMP_PIN             (GpioPin11)
#define BSP_ADC_HUMP_CHANAL          (AdcExInputCH18)

#define BSP_ADC_NTC_USE_10K

boolean_t Bsp_Adc_Start(int16_t *piTemp);

/* 获取当前点adc值 */
int Bsp_Adc_GetAdc(en_adc_samp_ch_sel_t enstcAdcSampCh);

#endif
