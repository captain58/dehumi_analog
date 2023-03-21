#ifndef __SRC_MAIN_H__
#define __SRC_MAIN_H__
#include <rtthread.h>
#include "bsp/bsp_wdt.h"
#define MAJOR_VER       "v0.0."
#define MINOR_VER       "43"
#define SPECIAL_VER     "-chr07"

/* 打印缓存 */
extern char g_cPrintTmp[100];

#define Debug_Printsub(_p)   {if (GetPrintFlag()) rt_kprintf(_p);}

/* 打印 */
#define Debug_Print(_fmt, ...)  \
            { \
                memset(g_cPrintTmp, 0, sizeof(g_cPrintTmp)); \
                snprintf(g_cPrintTmp, sizeof(g_cPrintTmp), "\r\nTime["MINOR_VER"] %u:%u --> "_fmt"\r\n", \
                        rt_tick_get()/1000, rt_tick_get()%1000, ##__VA_ARGS__); \
                Debug_Printsub(g_cPrintTmp);\
            }

#define Debug_fileline      Debug_Print("file:%s,line:%d", __FILE__, __LINE__);

/* 获取打印开关 */
uint8_t GetPrintFlag(void);

/* 打印hex字符 */
void PrintHexLog(uint8_t *pucChar, uint16_t ucLen, char *pcStr);

extern uint64_t g_Rs485OnlineLastTime;

#define __USE_WATERDUMP_GPIO__        /* 使用io控制，若不定义此宏，使用可控硅 */

#endif

