#include "bsp_wdt.h"

///< 启动 WDT
void Bsp_Wdt_Start(void) {
    Wdt_Start();
    return;
}

///< 喂狗
void Bsp_Wdt_Feed(void) {
    Wdt_Feed();
    return;
}

static int App_WdtInit(void)
{
    ///< 开启WDT外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralWdt,TRUE);
    ///< WDT 初始化
    Wdt_Init(WdtResetEn, WdtT26s2);
    return 0;
}

INIT_BOARD_EXPORT(App_WdtInit);
