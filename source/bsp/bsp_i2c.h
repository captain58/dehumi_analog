#ifndef __BSP_I2C_H__
#define __BSP_I2C_H__

#include "i2c.h"
#include "gpio.h"
#include <rtthread.h>
#define I2C_DEVADDR 0x44
boolean_t Bsp_I2c_WriteData(uint8_t *pu8Data,uint32_t u32Len);
boolean_t Bsp_I2c_ReadData(uint8_t *pu8Data,uint32_t u32Len, uint32_t uiTimeOut);

#endif