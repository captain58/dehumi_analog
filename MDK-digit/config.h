#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAJOR_VER       "v0.0."
#define MINOR_VER       "67"
#define SPECIAL_VER     "-SHT3"

/* 管盘温度adc */
#define BSP_ADC_NTC_GPIO            (GpioPortA)
#define BSP_ADC_NTC_PIN             (GpioPin0)
#define BSP_ADC_NTC_CHANAL          (AdcExInputCH0)

///* 温度adc */

#define BSP_ADC_TEMP_GPIO            (GpioPortB)
#define BSP_ADC_TEMP_PIN             (GpioPin1)
#define BSP_ADC_TEMP_CHANAL          (AdcExInputCH9)

/* 湿度adc */
#define BSP_ADC_HUMP_GPIO            (GpioPortB)
#define BSP_ADC_HUMP_PIN             (GpioPin1)
#define BSP_ADC_HUMP_CHANAL          (AdcExInputCH9)

/* RH_IO1 */
#define   RH_IO1_GPIO                (GpioPortB)
#define   RH_IO1_PIN                 (GpioPin13)

/* RH_IO2 */
#define   RH_IO2_GPIO                (GpioPortB)
#define   RH_IO2_PIN                 (GpioPin14)
#define CON_AC_MOTOR 1//交流电机
#endif

