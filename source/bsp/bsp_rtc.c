#include "bsp_rtc.h"

static Rtc_Alarm_Cb g_alarm_cb = NULL;

/**
******************************************************************************
    ** \brief  RTC中断入口函数
    ** 
  ** @param  无
    ** \retval 无
    **
******************************************************************************/  
void Rtc_IRQHandler(void)
{
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    if(Rtc_GetPridItStatus() == TRUE)
    {
        if (g_alarm_cb != NULL) {
            g_alarm_cb();
        }
        Rtc_ClearPrdfItStatus();             //清除中断标志位
    }
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}

void Bsp_Rtc_SetIrqCb(Rtc_Alarm_Cb cb) {
    g_alarm_cb = cb;
}

//RTC获取时间函数
en_result_t Bsp_Rtc_GetTime(stc_rtc_time_t* time) {
    en_result_t result;
    result = Rtc_ReadDateTime(time);
    if (time->u8DayOfWeek == 0) {
        time->u8DayOfWeek = 7;
    }
    return result;
}

/* 同步rtc时间并开始1分钟一次的中断 */
void Bsp_Rtc_Sync(uint8_t u8Year, uint8_t u8Month, uint8_t u8Day, uint8_t u8DayOfWeek, uint8_t u8Hour, uint8_t u8Minute, uint8_t u8Second) {
    stc_rtc_initstruct_t RtcInitStruct;
    RtcInitStruct.rtcAmpm = RtcPm;        //24小时制
    RtcInitStruct.rtcClksrc = RtcClkRcl;  //内部低速时钟
    RtcInitStruct.rtcPrdsel.rtcPrdsel = RtcPrds;  //周期中断类型PRDS
    RtcInitStruct.rtcPrdsel.rtcPrds = Rtc1Min;      //周期中断事件间隔
    RtcInitStruct.rtcTime.u8Second = u8Second;
    RtcInitStruct.rtcTime.u8Minute = u8Minute;
    RtcInitStruct.rtcTime.u8Hour   = u8Hour;
    RtcInitStruct.rtcTime.u8Day    = u8Day;
    RtcInitStruct.rtcTime.u8DayOfWeek = u8DayOfWeek;
    RtcInitStruct.rtcTime.u8Month  = u8Month;
    RtcInitStruct.rtcTime.u8Year   = u8Year;
    RtcInitStruct.rtcCompen = RtcCompenEnable;
    RtcInitStruct.rtcCompValue = 0;//补偿值根据实际情况进行补偿
    Rtc_Init(&RtcInitStruct);
    Rtc_AlmIeCmd(TRUE);                  //使能闹钟中断
    EnableNvic(RTC_IRQn, IrqLevel3, TRUE);                //使能RTC中断向量
    Rtc_Cmd(TRUE);                                        //使能RTC开始计数
}

static int Bsp_Rtc_Init(void) {
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);//GPIO外设时钟打开
    Sysctrl_SetPeripheralGate(SysctrlPeripheralRtc,TRUE);//RTC模块时钟打开
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    return 0;
}

INIT_BOARD_EXPORT(Bsp_Rtc_Init);
