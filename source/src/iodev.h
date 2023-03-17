#ifndef __SRC_IODEV_H__
#define __SRC_IODEV_H__

#include <rtthread.h>
#include "bsp/bsp_gpio.h"
#include "bsp/bsp_timer.h"
#include "bsp/bsp_elecheck.h"

/* 漏水 */
#define   WATER_LEAKAGE_GPIO        (GpioPortA)
#define   WATER_LEAKAGE_PIN         (GpioPin5)

/* 下水位 */
#define   LOW_WATER_LEVEL_GPIO      (GpioPortA)
#define   LOW_WATER_LEVEL_PIN       (GpioPin7)

/* 上水位 */
#define   UPPER_WATER_LEVEL_GPIO    (GpioPortA)
#define   UPPER_WATER_LEVEL_PIN     (GpioPin6)

/* 中水位 */
#define   MID_WATER_LEVEL_GPIO    (GpioPortA)
#define   MID_WATER_LEVEL_PIN     (GpioPin11)

/* 压缩机压力 */
#define   COMPERSSORPRESSURE_GPIO   (GpioPortA)
#define   COMPERSSORPRESSURE_PIN    (GpioPin1)

/* 压缩机 */
#define   COMPERSSOR_GPIO           (GpioPortB)
#define   COMPERSSOR_PIN            (GpioPin12)

/* 紫外灯 */
#define   UVLAMP_GPIO               (GpioPortB)
#define   UVLAMP_PIN                (GpioPin1)

/* 负离子 */
#define   ANION_GPIO                (GpioPortB)
#define   ANION_PIN                 (GpioPin0)

/* 臭氧 消毒 */
#define   OZONE_GPIO                (GpioPortA)
#define   OZONE_PIN                 (GpioPin8)

/* 百叶风机 */
#define   BAIYEFENGJI_GPIO                (GpioPortA)
#define   BAIYEFENGJI_PIN                 (GpioPin12)

/* 排水 */
#define   PAISHUI_GPIO                (GpioPortB)
#define   PAISHUI_PIN                 (GpioPin15)

/* LED */
#define   LED_GPIO                (GpioPortD)
#define   LED_PIN                 (GpioPin7)

/* RH_IO1 */
#define   RH_IO1_GPIO                (GpioPortB)
#define   RH_IO1_PIN                 (GpioPin13)

/* RH_IO2 */
#define   RH_IO2_GPIO                (GpioPortB)
#define   RH_IO2_PIN                 (GpioPin14)

boolean_t IoDevGetStatus(en_gpio_port_t enPort, en_gpio_pin_t enPin);
boolean_t IoDevGetElectricMachineryStatus(void);

/* 设置压缩机状态 */
void IoDevSetCompressor(uint8_t flag);
/* 设置加湿水泵状态 */
void IoDevSetWaterPump(uint8_t flag);
/* 设置紫外灯状态 */
void IoDevSetUVLamp(uint8_t flag);
/* 设置负离子状态 */
void IoDevSetAnion(uint8_t flag);
/* 设置电机状态 */
void IoDevSetElectricMachinery(uint8_t flag);
/* 设置臭氧状态 */
void IoDevSetOzone(uint8_t flag);
/* 设置排水状态 */
void IoDevSetPaishui(uint8_t flag);
/* 设置百叶风机状态 */
void IoDevSetBaiYe(uint8_t flag);
/* 设置LED状态 */
void IoDevSetLED(uint8_t flag);
/* 设置RH_IO1状态 */
void IoDevSetRH_IO1(uint8_t flag);
/* 设置RH_IO2状态 */
void IoDevSetRH_IO2(uint8_t flag);

#endif
