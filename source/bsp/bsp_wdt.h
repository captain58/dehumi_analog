#ifndef __BSP_WDT_H__
#define __BSP_WDT_H__

#include "wdt.h"
#include "lpm.h"
#include "gpio.h"
#include <rtthread.h>

///< 启动 WDT
void Bsp_Wdt_Start(void);

///< 喂狗
void Bsp_Wdt_Feed(void);
#endif
