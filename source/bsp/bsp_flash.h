#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

#include "ddl.h"
#include "flash.h"
#include <rtthread.h>
#include "main.h"

void Bsp_Flash_Init(void);

/* flash 读取数据 */
boolean_t Bsp_Flash_ReadData(uint8_t *pdata, uint16_t uiLen);

/* flash 写入数据 */
boolean_t Bsp_Flash_StoreData(uint8_t *pdata, uint16_t uiLen);

#endif
