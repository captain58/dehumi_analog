#ifndef __BSP_ADC_NTC_H__
#define __BSP_ADC_NTC_H__

#include "adc.h"
#include "gpio.h"
#include "bgr.h"
#include "config.h"
#include <rtthread.h>



#define BSP_ADC_NTC_USE_10K

boolean_t Bsp_Adc_Start(int16_t *piTemp);

/* 获取当前点adc值 */
int Bsp_Adc_GetAdc(en_adc_samp_ch_sel_t enstcAdcSampCh);

#endif
