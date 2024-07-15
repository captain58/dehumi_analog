#ifndef __BSP_PM2_5_H__
#define __BSP_PM2_5_H__

#include "gpio.h"
#include "bsp_timer.h"
#include "bsp_gpio.h"
#include <rtthread.h>
#include <stdlib.h>

#define BSP_PM2_5_DATA_GPIO         (GpioPortB)
#define BSP_PM2_5_DATA_PIN          (GpioPin11)
uint16_t Bsp_Pm2_5_Sync(void);
void Bsp_Pm2_5_IrqCb(void);
uint16_t Bsp_Pm2_5_Init();
#endif



