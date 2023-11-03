#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAJOR_VER       "v0.0."
#define MINOR_VER       "65"
#define SPECIAL_VER     "-CHR7"
/* �����¶�adc */
#define BSP_ADC_NTC_GPIO            (GpioPortA)
#define BSP_ADC_NTC_PIN             (GpioPin0)
#define BSP_ADC_NTC_CHANAL          (AdcExInputCH0)

/* �¶�adc */
#define BSP_ADC_TEMP_GPIO            (GpioPortB)
#define BSP_ADC_TEMP_PIN             (GpioPin10)
#define BSP_ADC_TEMP_CHANAL          (AdcExInputCH17)

/* ʪ��adc */
#define BSP_ADC_HUMP_GPIO            (GpioPortB)
#define BSP_ADC_HUMP_PIN             (GpioPin11)
#define BSP_ADC_HUMP_CHANAL          (AdcExInputCH18)

/* RH_IO1 */
#define   RH_IO1_GPIO                (GpioPortB)
#define   RH_IO1_PIN                 (GpioPin13)

/* RH_IO2 */
#define   RH_IO2_GPIO                (GpioPortB)
#define   RH_IO2_PIN                 (GpioPin14)

#define CON_AC_MOTOR 0
#endif

