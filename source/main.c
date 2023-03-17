#include "main.h"
#include "src/data.h"
#include "src/task.h"
#include "bsp/bsp_uart.h"

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** check Pxx to verify the clock frequency.
 **
 ******************************************************************************/

//	#define APP_MAIN_VERSION            ("v0.0.41-chr07")
#define APP_MAIN_VERSION (MAJOR_VER""MINOR_VER""SPECIAL_VER)

static uint8_t g_recoverflag = 1;

void RecoverMode(void) {
    ALARM_DATA_S stalarminfo;
    CORE_DATA_S *pstData = NULL;

    if (g_recoverflag == 0) {
        return;
    }

    g_recoverflag = 0;
    memset(&stalarminfo, 0, sizeof(stalarminfo));
    Data_Get_Lock();
    pstData = Data_Get_Point();
    memcpy(&stalarminfo, &pstData->stAlarmData, sizeof(stalarminfo));
    Data_Get_UnLock();
    if (stalarminfo.uiLastRunMode == 2) {
        Debug_Print("Sync last mode:%u", stalarminfo.stLastOutPutEnable.uiMode);
        Run_Mode_Set(0, &stalarminfo.stLastOutPutEnable, 1);
    }
    return;
}

int32_t main(void)
{
    Bsp_Wdt_Start();
    rt_thread_mdelay(2000);
    Debug_Print("-------->app version:%s", APP_MAIN_VERSION);
    while (1)
    {
        Debug_Print("main tick:%u\n", rt_tick_get());
        rt_thread_mdelay(10000);
        Bsp_Wdt_Feed();     /* 喂狗 */
        RecoverMode();
    }
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/



