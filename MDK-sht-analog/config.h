#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAJOR_VER       "v0.0."
#define MINOR_VER       "66"
#define SPECIAL_VER     "-SHT3x-ARP"
/* �����¶�adc */
#define BSP_ADC_NTC_GPIO            (GpioPortA)
#define BSP_ADC_NTC_PIN             (GpioPin0)
#define BSP_ADC_NTC_CHANAL          (AdcExInputCH0)

/* �¶�adc */
#define BSP_ADC_TEMP_GPIO            (GpioPortB)
#define BSP_ADC_TEMP_PIN             (GpioPin14)
#define BSP_ADC_TEMP_CHANAL          (AdcExInputCH21)

/* ʪ��adc */
#define BSP_ADC_HUMP_GPIO            (GpioPortB)
#define BSP_ADC_HUMP_PIN             (GpioPin13)
#define BSP_ADC_HUMP_CHANAL          (AdcExInputCH20)

/* RH_IO1 */
#define   RH_IO1_GPIO                (GpioPortD)
#define   RH_IO1_PIN                 (GpioPin6)

/* RH_IO2 */
#define   RH_IO2_GPIO                (GpioPortD)
#define   RH_IO2_PIN                 (GpioPin7)

#define CON_AC_MOTOR 0
#endif

