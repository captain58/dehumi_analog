#ifndef __BSP_DHT11_H__
#define __BSP_DHT11_H__

// #define __BPS_USE_DHT11_H__

#ifdef __BPS_USE_DHT11_H__
#include "gpio.h"
#include "bsp_timer.h"
#include "bsp_gpio.h"
#include <rtthread.h>

#define BSP_DHT11_DATA_GPIO         (GpioPortB)
#define BSP_DHT11_DATA_PIN          (GpioPin13)
void Bsp_Dht11_Sync(void);
void Bsp_Dht11_IrqCb(void);
#endif

#endif
