#ifndef __BSP_ELECHECK_H__
#define __BSP_ELECHECK_H__

#include "gpio.h"
#include "bsp_timer.h"
#include "bsp_gpio.h"
#include <rtthread.h>

#define   ELEMACHFG_GPIO   (GpioPortB)
#define   ELEMACHFG_PIN    (GpioPin4)
uint8_t Bsp_EleCheck_Sync(void);

#endif
