#ifndef __SRC_TASK_H__
#define __SRC_TASK_H__

#include <rtthread.h>
#include "main.h"
#include "src/data.h"
#include "bsp/bsp_flash.h"

typedef enum pm2_5_level {
    PM2_5_LEVEL_NONE = 0,        /* 不修正 */
    PM2_5_LEVEL_GOOD,           /* 优 */
    PM2_5_LEVEL_BAD,            /* 劣 */
    PM2_5_LEVEL_GEN,            /* 良 */
}PM2_5_LEVEL_E;

/* 获取pm2.5的等级 */
uint8_t Get_Pm2_5_Level(uint16_t value);

/* 获取co2的等级 */
uint8_t Get_CO2_Level(uint16_t value);

/* 获取甲醛的等级 */
uint8_t Get_formaldehyde_Level(uint16_t value);

/* 模式运行设置 */
void Run_Mode_Set(uint8_t uiflag, DIGIT_STATUS_U *pstParaCmd, uint8_t uiResetFlag);
#endif
