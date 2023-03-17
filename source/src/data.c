#include "data.h"
#include "task.h"
#include "temp.h"
#pragma pack(1)

static CORE_DATA_S g_Core_Data;
char g_cPrintTmp[100];

/* 邮箱控制块 */
static struct rt_mailbox tswitch_mb;
/* 用于放邮件的内存池 */
static char tswitch_mb_pool[32];

/* 打印hex字符 */
void PrintHexLog(uint8_t *pucChar, uint16_t ucLen, char *pcStr) {
    uint16_t ucLoop = 0;

    if (GetPrintFlag() == 0)
        return;
    
    rt_kprintf("\n %s : ", pcStr);
    for (ucLoop = 0; ucLoop < ucLen; ucLoop++) {
        rt_kprintf(" 0x%02x", pucChar[ucLoop]);
    }
    rt_kprintf("\n");
    return;
}

/* 获取打印开关 */
uint8_t GetPrintFlag(void) {
    return g_Core_Data.stDevStatus.uiPrintFlag;
}

CORE_DATA_S *Data_Get_Point(void) {
    return &g_Core_Data;
}

void Data_Get_Lock(void) {
    rt_mutex_take(&g_Core_Data.coredatametux, RT_WAITING_FOREVER);
}

void Data_Get_UnLock(void) {
    rt_mutex_release(&g_Core_Data.coredatametux);
}

/* 获取风扇开关 */
uint8_t Get_Ele_Enable(void) {
    uint8_t temp;
    Data_Get_Lock();
    temp = g_Core_Data.stOutPutDigit.uiElectricMachinery;
    Data_Get_UnLock();
    return temp;
}

/* 获取设备运行模式 */
RUN_MODE_E Get_Dev_RunMode(void) {
    RUN_MODE_E temp;
    Data_Get_Lock();
    temp = g_Core_Data.stDevStatus.uiDevRunMode;
    Data_Get_UnLock();
    return temp;
}

/* 设置设备运行模式 */
void Set_Dev_RunMode(RUN_MODE_E uiMode) {
    Data_Get_Lock();
    g_Core_Data.stDevStatus.uiDevRunMode = uiMode;
    Data_Get_UnLock();
    return;
}

/* 获取设备运行状态 */
RUN_STATUS_E Get_Dev_RunStatus(void) {
    RUN_STATUS_E temp;
    Data_Get_Lock();
    temp = g_Core_Data.stDevStatus.uiDevRunStatus;
    Data_Get_UnLock();
    return temp;
}

/* 设置设备运行状态 */
void Set_Dev_RunStatus(RUN_STATUS_E uiStatus) {
    Data_Get_Lock();
    g_Core_Data.stDevStatus.uiDevRunStatus = uiStatus;
    Data_Get_UnLock();
    return;
}

/* 获取设备运行子状态 */
uint8_t Get_Dev_RunSubStatus(void) {
    uint8_t temp;
    Data_Get_Lock();
    temp = g_Core_Data.stDevStatus.uiDevRunSubStatus;
    Data_Get_UnLock();
    return temp;
}

/* 设置设备运行子状态 */
void Set_Dev_RunSubStatus(uint8_t uiStatus) {
    Data_Get_Lock();
    g_Core_Data.stDevStatus.uiDevRunSubStatus = uiStatus;
    Data_Get_UnLock();
    return;
}

/* 获取设备定时开关状态 0关 1开 */
uint8_t Get_Dev_TimRunStatus(void) {
    uint8_t temp;
    Data_Get_Lock();
    temp = g_Core_Data.stAlarmData.uiTimOpenCloseFlag;
    Data_Get_UnLock();
    return temp;
}

/* 设置设备定时开关状态   0关 1开 */
void Set_Dev_TimRunStatus(uint8_t uiFlag) {
    Data_Get_Lock();
    if (uiFlag) {
        g_Core_Data.stAlarmData.uiTimOpenCloseFlag = 1;
    } else {
        g_Core_Data.stAlarmData.uiTimOpenCloseFlag = 0;
        g_Core_Data.stAlarmData.uiTimOpenCloseMap = 0;
    }
    Data_Get_UnLock();
    return;
}

/* 获取设备部件使能状态 */
void Get_Dev_RunEnable(DIGIT_STATUS_U *pstEnable) {
    memset(pstEnable, 0, sizeof(DIGIT_STATUS_U));
    Data_Get_Lock();
    memcpy(pstEnable, &g_Core_Data.stOutPutEnable, sizeof(DIGIT_STATUS_U));
    Data_Get_UnLock();
    return;
}

/* 设置设备部件使能状态 */
void Set_Dev_RunEnable(DIGIT_STATUS_U *pstEnable) {
    Data_Get_Lock();
    memcpy(&g_Core_Data.stOutPutEnable, pstEnable, sizeof(DIGIT_STATUS_U));
    Data_Get_UnLock();
    return;
}

/* 设置部件当前状态 */
void Set_Dev_RunDiGit(DIGIT_STATUS_U *pstDiGit) {
    Data_Get_Lock();
    memcpy(&g_Core_Data.stOutPutDigit, pstDiGit, sizeof(DIGIT_STATUS_U));
    Data_Get_UnLock();
    return;
}

/* 获取部件当前状态 */
void Get_Dev_RunDiGit(DIGIT_STATUS_U *pstDiGit) {
    memset(pstDiGit, 0, sizeof(DIGIT_STATUS_U));
    Data_Get_Lock();
    memcpy(pstDiGit, &g_Core_Data.stOutPutDigit, sizeof(DIGIT_STATUS_U));
    Data_Get_UnLock();
    return;
}

/* 保存核心数据 */
void Store_Core_Data2Flash(void) {
    Data_Get_Lock();
    Bsp_Flash_StoreData((uint8_t *)&g_Core_Data.stAlarmData, sizeof(g_Core_Data.stAlarmData));
    Data_Get_UnLock();
    return;
}

/* 读取核心数据 */
void Get_Core_DataFromFlash(void) {
    Data_Get_Lock();
    Bsp_Flash_ReadData((uint8_t *)&g_Core_Data.stAlarmData, sizeof(g_Core_Data.stAlarmData));
    Data_Get_UnLock();
    return;
}

void StoreRtcTime(uint8_t u8Year, uint8_t u8Month, uint8_t u8Day, uint8_t u8DayOfWeek, uint8_t u8Hour, uint8_t u8Minute, uint8_t u8Second) {
    Data_Get_Lock();
    g_Core_Data.stAlarmData.stRtcTime.uiYear = u8Year;
    g_Core_Data.stAlarmData.stRtcTime.uiMon = u8Month;
    g_Core_Data.stAlarmData.stRtcTime.uiDay = u8Day;
    g_Core_Data.stAlarmData.stRtcTime.uiHour = u8Hour;
    g_Core_Data.stAlarmData.stRtcTime.uiMin = u8Minute;
    g_Core_Data.stAlarmData.stRtcTime.uiSec = u8Second;
    g_Core_Data.stAlarmData.stRtcTime.uiWeek = u8DayOfWeek;
    g_Core_Data.stAlarmData.stRtcTime.uiSyncFlag = 1;
    Data_Get_UnLock();

    Store_Core_Data2Flash();
    return;
}

/* 根据星期获取该天的定时类型 */
TIM_OPENCLOSE_TYPE_E Get_TimTypeByWeek(uint8_t uiWeek) {
    uint8_t uiRet = TIM_OPENCLOSE_BUTT;

    if ((uiWeek < 1) || (uiWeek > 7)) {
        return uiRet;
    }

    uiWeek -= 1;
    uiWeek *= 2;
    Data_Get_Lock();
    uiRet = (g_Core_Data.stAlarmData.uiTimOpenCloseMap >> uiWeek) & 0x3;
    Data_Get_UnLock();
    return uiRet;
}

/* 获取当前是单次定时还是循环定时，0单次，1循环 */
uint8_t Get_TimCycleType(void) {
    uint8_t uiRet = 0;
    Data_Get_Lock();
    if ((g_Core_Data.stAlarmData.uiTimOpenCloseMap >> 14) & 0x1) {
        uiRet = 1;
    }
    Data_Get_UnLock();
    return uiRet;
}

/* 根据星期设置该天的定时类型 */
void Set_TimTypeByWeek(uint8_t uiWeek, TIM_OPENCLOSE_TYPE_E enType) {
    if ((uiWeek < 1) || (uiWeek > 7) || (enType >= TIM_OPENCLOSE_BUTT)) {
        return;
    }

    uiWeek -= 1;
    uiWeek *= 2;
    Data_Get_Lock();
    g_Core_Data.stAlarmData.uiTimOpenCloseMap &= (~(3 << uiWeek));
    g_Core_Data.stAlarmData.uiTimOpenCloseMap |= (enType << uiWeek);
    Data_Get_UnLock();
    Store_Core_Data2Flash();
    return;
}

/* 设置当前是单次定时还是循环定时，0单次，1循环 */
void Set_TimCycleType(uint8_t uiFlag) {
    if (uiFlag) {
        Data_Get_Lock();
        g_Core_Data.stAlarmData.uiTimOpenCloseMap |= (1 << 14);
        Data_Get_UnLock();
    } else {
        Data_Get_Lock();
        g_Core_Data.stAlarmData.uiTimOpenCloseMap &= (~(1 << 14));
        Data_Get_UnLock();
    }
    Store_Core_Data2Flash();
}

/* 判断定时是否结束（单次循环且所有模式都是常关）若关闭，返回1且存flash，否则返回0 */
uint8_t Check_TimIsClosed(void) {
    uint8_t uiRet = 0;

    Data_Get_Lock();
    if (g_Core_Data.stAlarmData.uiTimOpenCloseMap == 0) {
        g_Core_Data.stAlarmData.uiTimOpenCloseFlag = 0;
        uiRet = 1;
    }
    Data_Get_UnLock();
    if (uiRet)
        Store_Core_Data2Flash();
    return uiRet;
}

/* 获取设备显示运行状态 */
RUN_STATUS_E Get_DevStatusForDisplay(void) {
    uint8_t uiRet = 0;
    uint8_t uiSubStatus = 0;
    
    Data_Get_Lock();
    uiRet = g_Core_Data.stDevStatus.uiDevRunMode;
    if (uiRet == RUN_MODE_STANDBY_E) {
        uiRet = 4;
    } else if (uiRet == RUN_MODE_STOP_E) {
        uiRet = 5;
    } else {
        uiRet -= 1;
    }

    Data_Get_UnLock();
    return uiRet;
}

/* rtc定时中断阻塞，1分钟一次 */
void TimeSwitch_thread_entry(void *parameter) {
    char *str;
    CORE_DATA_S *pstData = NULL;
    stc_rtc_time_t curtime;
    uint8_t uiTimFlag = 0;          /* 定时开关标志位 */
    DATA_TIME_S    stDataTimeStart[3];         /* 定时开始时间日期 */
    DATA_TIME_S    stDataTimeEnd[3];           /* 定时结束时间日期 */
    DIGIT_STATUS_U stOutPutEnable;          /* 定时开关的部件操作状态 */

    while (1)
    {
        /* 从邮箱中收取邮件 */
        if (rt_mb_recv(&tswitch_mb, (rt_ubase_t *)&str, RT_WAITING_FOREVER) == RT_EOK) {
            Bsp_Rtc_GetTime(&curtime);
            Debug_Print("get cur rtc time: %02x-%02x-%02x %02x:%02x:%02x (week:%02x)", 
                curtime.u8Year, curtime.u8Month, curtime.u8Day, curtime.u8Hour, curtime.u8Minute, curtime.u8Second, curtime.u8DayOfWeek);
            StoreRtcTime(curtime.u8Year, curtime.u8Month, curtime.u8Day, curtime.u8DayOfWeek,
                         curtime.u8Hour, curtime.u8Minute, curtime.u8Second);
            Data_Get_Lock();
            pstData = Data_Get_Point();
            if ((pstData->stDevStatus.uiDevRunStatus == RUN_STATUS_DEHUMIDIFICATION_ING) ||
                (pstData->stDevStatus.uiDevRunStatus == RUN_STATUS_HUMIDIFICATION_ING)) {
                pstData->stAlarmData.uiUpdateWaterTimeCnt++;
                if (pstData->stDevStatus.uiDevRunStatus == RUN_STATUS_HUMIDIFICATION_ING) {
                    pstData->stAlarmData.uiwetfilmChangeTimeCnt++;
                }
            }

            if ((pstData->stDevStatus.uiDevRunStatus != RUN_STATUS_STANDBY) &&
                (pstData->stDevStatus.uiDevRunStatus != RUN_STATUS_STOP)) {
                pstData->stAlarmData.uiFilterChangeTimeCnt++;
            }

            /* 判断换水时间 */
            if (pstData->stAlarmData.uiUpdateWaterTimeCnt > pstData->stAlarmData.uiUpdateWaterTime * 60 * 24) {
                pstData->stDevStatus.uiupdatewaterStatus = 1;
            }
            /* 判断滤网时间 */
            if (pstData->stAlarmData.uiFilterChangeTimeCnt > pstData->stAlarmData.uiFilterChangeTime * 60 * 24) {
                pstData->stDevStatus.uiFilterStatus = 1;
            }
            /* 判断湿膜时间 */
            if (pstData->stAlarmData.uiwetfilmChangeTimeCnt > pstData->stAlarmData.uiwetfilmChangeTime * 60 * 24) {
                pstData->stDevStatus.uiwetfilmStatus = 1;
            }

            if (pstData->stDevStatus.uiTimeSync) {
                /* 屏幕一小时保存一次数据 */
                if (curtime.u8Minute == 0) {
                    pstData->stDevStatus.uiStoreData = 1;
                }
                uiTimFlag = pstData->stAlarmData.uiTimOpenCloseFlag;
                if (uiTimFlag) {
                    memcpy(stDataTimeStart, pstData->stAlarmData.stDataTimeStart, sizeof(stDataTimeStart));
                    memcpy(stDataTimeEnd, pstData->stAlarmData.stDataTimeEnd, sizeof(stDataTimeEnd));
                    memcpy(&stOutPutEnable, &pstData->stAlarmData.stOutPutEnable, sizeof(stOutPutEnable));
                }
            } else {
                uiTimFlag = 0;
            }
            Data_Get_UnLock();

            /* 定时开关开启 */
            if ((uiTimFlag) && (Ok == Bsp_Rtc_GetTime(&curtime))) {
                uint8_t uiCurDayType = Get_TimTypeByWeek(curtime.u8DayOfWeek);
                
                /* 当天的第一分钟，一天操作一次的内容在这里 */
                if ((curtime.u8Hour == 0) && (curtime.u8Minute == 1)) {
                    /* 循环若为单次，关闭昨天的循环 */
                    if (0 == Get_TimCycleType()) {
                        if (curtime.u8DayOfWeek == 1) {
                            Set_TimTypeByWeek(7, TIM_OPENCLOSE_F4);
                        } else {
                            Set_TimTypeByWeek(curtime.u8DayOfWeek - 1, TIM_OPENCLOSE_F4);
                        }

                        /* 检测是否需要关闭定时 */
                        Check_TimIsClosed();
                    }
                }

                if (uiCurDayType == TIM_OPENCLOSE_F1) {
                    uint8_t uiStartEnable = 0;
                    uint8_t uiEndEnable = 2;
                    if ((stDataTimeStart[0].uiHour == stDataTimeEnd[0].uiHour) && (stDataTimeStart[0].uiMin == stDataTimeEnd[0].uiMin)) {
                        uiStartEnable = 1;
                    }
                    if ((stDataTimeStart[1].uiHour == stDataTimeEnd[1].uiHour) && (stDataTimeStart[1].uiMin == stDataTimeEnd[1].uiMin)) {
                        uiEndEnable = 1;
                    }

                    for (int i = uiStartEnable; i < uiEndEnable; i++) {
                        /* 定时开启 */
                        if ((stDataTimeStart[i].uiHour == curtime.u8Hour) && (stDataTimeStart[i].uiMin == curtime.u8Minute)) {
                            Debug_fileline
                            Run_Mode_Set(0, &stOutPutEnable, 0);
                        }
                        /* 定时关闭 */
                        if ((stDataTimeEnd[i].uiHour == curtime.u8Hour) && (stDataTimeEnd[i].uiMin == curtime.u8Minute)) {
                            Debug_fileline
                            Run_Mode_Set(1, &stOutPutEnable, 0);

                            /* 最后一次时间段判断是否需要结束定时 */
                            if ((i == (uiEndEnable - 1)) && (0 == Get_TimCycleType())) {
                                Set_TimTypeByWeek(curtime.u8DayOfWeek, TIM_OPENCLOSE_F4);
                                Check_TimIsClosed();
                            }
                        }
                    }

                    /* 2个时间段都相等，直接改为常关模式 */
                    if (uiStartEnable == uiEndEnable) {
                        Set_TimTypeByWeek(curtime.u8DayOfWeek, TIM_OPENCLOSE_F4);
                        Check_TimIsClosed();
                    }
                } 
                
                if (uiCurDayType == TIM_OPENCLOSE_F2) {
                    /* 若开关时间相等，不生效 */
                    if ((stDataTimeStart[2].uiHour != stDataTimeEnd[2].uiHour) || (stDataTimeStart[2].uiMin != stDataTimeEnd[2].uiMin)) {
                        /* 定时开启 */
                        if ((stDataTimeStart[2].uiHour == curtime.u8Hour) && (stDataTimeStart[2].uiMin == curtime.u8Minute)) {
                            Debug_fileline
                            Run_Mode_Set(0, &stOutPutEnable, 0);
                        }
                        /* 定时关闭 */
                        if ((stDataTimeEnd[2].uiHour == curtime.u8Hour) && (stDataTimeEnd[2].uiMin == curtime.u8Minute)) {
                            Debug_fileline
                            Run_Mode_Set(1, &stOutPutEnable, 0);
                            
                            /* 判断是否需要结束定时 */
                            if (0 == Get_TimCycleType()) {
                                Set_TimTypeByWeek(curtime.u8DayOfWeek, TIM_OPENCLOSE_F4);
                                Check_TimIsClosed();
                            }
                        }
                    } else {    /* 时间相等，直接改为常关模式 */
                        Set_TimTypeByWeek(curtime.u8DayOfWeek, TIM_OPENCLOSE_F4);
                        Check_TimIsClosed();
                    }
                }
                
                uint8_t uiRunStatus = Get_Dev_RunStatus();
                if ((uiCurDayType == TIM_OPENCLOSE_F3) && (uiRunStatus == RUN_STATUS_STANDBY)) {
                    Debug_fileline
                    Run_Mode_Set(0, &stOutPutEnable, 0);
                }
                
                if ((uiCurDayType == TIM_OPENCLOSE_F4) && (uiRunStatus != RUN_STATUS_STANDBY) && (uiRunStatus != RUN_STATUS_STOP)) {
                    Debug_fileline
                    Run_Mode_Set(1, &stOutPutEnable, 0);
                }
            }
            
            /* 一小时备份一下flash */
            if (curtime.u8Minute == 0) {
                Store_Core_Data2Flash();
            }
        }
    }
}

void Pm2_5_thread_entry(void *parameter) {
    uint16_t uiPm2;
    uint16_t uiPm2_temp;
    double fPm2;
    uint16_t uiLoop = 0;

    while (TRUE) {
        uiLoop++;
        uiPm2 = Bsp_Pm2_5_Sync();
        fPm2 = uiPm2 / 10;
        uiPm2_temp = (uint16_t)(((-0.0058 * fPm2 * fPm2 * fPm2)) + (0.42 * fPm2 * fPm2) + 11.5*fPm2 + 5);
        if ((uiLoop % 10) == 0) {
            Debug_Print(">>>Bsp_Pm2_5_Sync: %u um/g\n", uiPm2_temp);
        }
        Data_Get_Lock();
        g_Core_Data.stInPutInfo.uiPm2_5 = uiPm2_temp;
        Data_Get_UnLock();
        rt_thread_mdelay(1000);
    }
}

/* 判断告警以及是否需要停机 */
void Check_Alarm_StopDevice(void) {
    uint8_t uiRet = 0;

    Data_Get_Lock();

    /* 温度小于-5，告警 */
    if (g_Core_Data.stInPutInfo.iTemperature < ENV_TEMPERATURE_STOP) {
        g_Core_Data.stDevStatus.uiTemperatureStatus = 1;
    }

    /* 温度持续1分钟大于40，告警 */
    if (g_Core_Data.stInPutInfo.iTemperature > ENV_TEMPERATURE_STOP_HIGH) {
        g_Core_Data.stDevStatus.uiHighTemperCnt++;
    } else {
        g_Core_Data.stDevStatus.uiHighTemperCnt = 0;
    }
    if (g_Core_Data.stDevStatus.uiHighTemperCnt > ENV_TEMPERATURE_STOP_HIGH_TIME) {
        g_Core_Data.stDevStatus.uiTemperatureStatus = 1;
    }

    /* 漏水，停机 */
    if (g_Core_Data.stInPutInfo.uiWaterLeakage) {
        uiRet = 1;
    }
    
    /* 温湿度传感器持续3分钟无法获取数据，停机 */
    if (g_Core_Data.stDevStatus.uiStopTemperCnt > 3 * 60) {
        uiRet = 1;
    }
    Data_Get_UnLock();

    if (uiRet) {
        Debug_Print(">>>Alarm!!! now stop device.");
        Set_Dev_RunStatus(RUN_STATUS_STOP);
        Set_Dev_RunMode(RUN_MODE_STOP_E);
    }
}

void Coredata_thread_entry(void *parameter) {
    int16_t iTubeTemperature;
    int16_t iTemperature=0;
    uint16_t uiHumidity=1500;
    uint16_t uiLoop = 0;

    while (TRUE) {
        uiLoop++;
        if (Sync_TempHum(&iTemperature, &uiHumidity) && (iTemperature > -3000)) {
            Data_Get_Lock();
            g_Core_Data.stInPutInfo.uiHumidity = uiHumidity + g_Core_Data.stAlarmData.iHumidityCorrect;
            g_Core_Data.stInPutInfo.iTemperature = iTemperature + g_Core_Data.stAlarmData.iTemperatureCorrect;
            g_Core_Data.stDevStatus.uiTemperatureStatus = 0;
            g_Core_Data.stDevStatus.uiHumidityStatus = 0;
            g_Core_Data.stDevStatus.uiStopTemperCnt = 0;
            Data_Get_UnLock();
            if ((uiLoop % 30) == 0) {
                Debug_Print(">>>Sync_TempHum: temp:%d, humi:%u\n", iTemperature, uiHumidity);
            }
        } else {
            Data_Get_Lock();
            g_Core_Data.stDevStatus.uiTemperatureStatus = 1;
            g_Core_Data.stDevStatus.uiHumidityStatus = 1;
            g_Core_Data.stDevStatus.uiStopTemperCnt++;
            Data_Get_UnLock();
            Debug_Print(">>>Sync_TempHum: failed\n");
        }

        if (Bsp_Adc_Start(&iTubeTemperature) && (iTubeTemperature > -2700)) {
            if ((uiLoop % 30) == 0) {
                Debug_Print(">>>Bsp_Adc_Start: temp:%d\n", iTubeTemperature);
            }
            Data_Get_Lock();
            g_Core_Data.stInPutInfo.iTubeTemperature = iTubeTemperature + g_Core_Data.stAlarmData.iTubeTemperCorrect;
            g_Core_Data.stDevStatus.uiTubeStatus = 0;
            Data_Get_UnLock();
        } else {
            Data_Get_Lock();
            g_Core_Data.stDevStatus.uiTubeStatus = 1;
            Data_Get_UnLock();
            Debug_Print(">>>Bsp_Adc_Start: failed\n");
        }

        Data_Get_Lock();
        g_Core_Data.stInPutInfo.uiWaterLeakage = IoDevGetStatus(WATER_LEAKAGE_GPIO, WATER_LEAKAGE_PIN);
        g_Core_Data.stInPutInfo.uiLowerWaterLevel = !IoDevGetStatus(LOW_WATER_LEVEL_GPIO, LOW_WATER_LEVEL_PIN);
        g_Core_Data.stInPutInfo.uiUpperWaterLevel = IoDevGetStatus(UPPER_WATER_LEVEL_GPIO, UPPER_WATER_LEVEL_PIN);
        g_Core_Data.stInPutInfo.uiMidWaterLevel = IoDevGetStatus(MID_WATER_LEVEL_GPIO, MID_WATER_LEVEL_PIN);
        g_Core_Data.stInPutInfo.uiCompressorPressure = !IoDevGetStatus(COMPERSSORPRESSURE_GPIO, COMPERSSORPRESSURE_PIN);
        if ((uiLoop % 30) == 0) {
            Debug_Print(">>>InputGet: uiWaterLeakage:%u\n", g_Core_Data.stInPutInfo.uiWaterLeakage);
            Debug_Print(">>>InputGet: uiLowerWaterLevel:%u\n", g_Core_Data.stInPutInfo.uiLowerWaterLevel);
            Debug_Print(">>>InputGet: uiUpperWaterLevel:%u\n", g_Core_Data.stInPutInfo.uiUpperWaterLevel);
            Debug_Print(">>>InputGet: uiMidWaterLevel:%u\n", g_Core_Data.stInPutInfo.uiMidWaterLevel);
            Debug_Print(">>>InputGet: uiCompressorPressure:%u\n", g_Core_Data.stInPutInfo.uiCompressorPressure);
        }
        
        if (g_Core_Data.stInPutInfo.uiCompressorPressure) {
            g_Core_Data.stDevStatus.uiComPreasure = 1;
        } else {
            g_Core_Data.stDevStatus.uiComPreasure = 0;
        }

        /* 判断排水状态 */
        if (g_Core_Data.stAlarmData.uiPaiShuiMode == 0) {   /* 自动排水 */
            if (g_Core_Data.stInPutInfo.uiUpperWaterLevel) {
                IoDevSetPaishui(1);
            }
            if (g_Core_Data.stInPutInfo.uiMidWaterLevel == 0) {
                IoDevSetPaishui(0);
            }
        } else {    /* 手动排水 */
            if (g_Core_Data.stDevStatus.uiPaiShuiStatus) {
                if (g_Core_Data.stInPutInfo.uiLowerWaterLevel) {    /* 低水位告警，关闭手动模式 */
                    IoDevSetPaishui(0);
                    g_Core_Data.stDevStatus.uiPaiShuiStatus = 0;
                } else {
                    IoDevSetPaishui(1);
                }
            } else {
                IoDevSetPaishui(0);
            }
        }
        
        Data_Get_UnLock();
        // if (Get_Ele_Enable() != ELEMAC_LEVEL_CLOSE) {
        //     g_Core_Data.stDevStatus.uiFanStatus = !IoDevGetElectricMachineryStatus();
        // }

        /* 判断是否需要停机 */
        Check_Alarm_StopDevice();

        rt_thread_mdelay(1000);
    }
}

void Rtc_Cb_Func(void) {
    /* 发送邮箱 */
    rt_mb_send(&tswitch_mb, NULL);
}

/* 修改系统初始化默认值 */
void CoreData_Default_Init(void) {
    memset(&g_Core_Data, 0, sizeof(g_Core_Data));
    g_Core_Data.stDevStatus.uiPrintFlag = 1;        /* 开启打印 */
    g_Core_Data.stDevStatus.uiDevRunStatus = RUN_STATUS_STANDBY;    /* 待机 */

    /* 阈值初始化 */
    g_Core_Data.stAlarmData.uiPm2_5Correct = 0;       /* 不修正 */
    g_Core_Data.stAlarmData.iTemperatureCorrect = 0;       /* 不修正 */
    g_Core_Data.stAlarmData.iTubeTemperCorrect = 0;       /* 不修正 */
    g_Core_Data.stAlarmData.iHumidityCorrect = 0;       /* 不修正 */
    g_Core_Data.stAlarmData.uiTimOpenCloseFlag = 0;       /* 定时开关机默认关闭 */
    g_Core_Data.stAlarmData.uiTimOpenCloseMap = 0;       /* 定时开关机默认关闭 */
    g_Core_Data.stAlarmData.uiHumidityLow = 4000;       /* 湿度下限 40% */
    g_Core_Data.stAlarmData.uiHumidityHigh = 6000;      /* 湿度上限 60% */
    g_Core_Data.stAlarmData.uiDehumiTime = 20;      /* 压缩机启动时间 */
    g_Core_Data.stAlarmData.uiDehumiTimeDelay = 10; /* 压缩机休息时间  */
    g_Core_Data.stAlarmData.iFrostingStartThr = -100;       /* 化霜开始时间 -1° */
    g_Core_Data.stAlarmData.iFrostingEndThr = 800;      /* 化霜结束时间 8° */
    g_Core_Data.stAlarmData.uiFrostingTime = 15;   /* 化霜时间 */
    g_Core_Data.stAlarmData.uiFilterChangeTime = 180;   /* 滤网更换时间 */
    g_Core_Data.stAlarmData.uiwetfilmChangeTime = 180;   /* 湿膜更换时间 */
    g_Core_Data.stAlarmData.uiUpdateWaterTime = 7;   /* 换水时间 */
    g_Core_Data.stDevStatus.uiHighTemperCnt = 0;    /* 高温计数 */
    g_Core_Data.stAlarmData.uiPaiShuiMode = 1;      /* 默认手动排水 */
    g_Core_Data.stDevStatus.uiPaiShuiStatus = 0;    /* 默认关闭手动排水 */
    g_Core_Data.stAlarmData.iLastStopTime = -3*60;  /* 默认上电即可打开压缩机 */

    Bsp_Rtc_SetIrqCb(Rtc_Cb_Func);
    Bsp_Rtc_Sync(0x0, 0x1, 0x1, 0x6, 0x0, 0x0, 0);
}

#define COREDATA_THREAD_STACK_SIZE            (512)
#define COREDATA_THREAD_NAME                  "data_t"
#define COREDATA_THREAD_PRIORITY               (20)
static struct rt_thread g_coredata_thread;
static char g_coredata_thread_stack[COREDATA_THREAD_STACK_SIZE];

#define PM2_5_THREAD_STACK_SIZE            (384)
#define PM2_5_THREAD_NAME                  "pm2_5_t"
#define PM2_5_THREAD_PRIORITY               (20)
static struct rt_thread g_pm2_5_thread;
static char g_pm2_5_thread_stack[PM2_5_THREAD_STACK_SIZE];

#define TIME_SWITCH_THREAD_STACK_SIZE            (512)
#define TIME_SWITCH_THREAD_NAME                  "t_switch_t"
#define TIME_SWITCH_THREAD_PRIORITY               (20)
static struct rt_thread g_time_switch_thread;
static char g_time_switch_thread_stack[TIME_SWITCH_THREAD_STACK_SIZE];

int Coredata_Pro_Init(void) {
    rt_err_t result = RT_EOK;
    ALARM_DATA_S stalarminfo;

    /* 设置默认值 */
    CoreData_Default_Init();

    result = rt_mutex_init(&g_Core_Data.coredatametux, "core_m", RT_IPC_FLAG_PRIO);
    if (result != RT_EOK) {
        Debug_Print(">>>rt_mutex_init coremutex: failed\n");
        return -1;
    }

    /* 初始化flash */
    Bsp_Flash_Init();

    /* flash获取设置值 */
    Get_Core_DataFromFlash();
    
    result = rt_thread_init(&g_coredata_thread,
                            COREDATA_THREAD_NAME,
                            Coredata_thread_entry, RT_NULL,
                            &g_coredata_thread_stack[0], sizeof(g_coredata_thread_stack),
                            COREDATA_THREAD_PRIORITY, 10);

    if (result == RT_EOK) {
        rt_thread_startup(&g_coredata_thread);
    } else {
        Debug_Print(">>>rt_thread_init %s: failed\n", COREDATA_THREAD_NAME);
        return -1;
    }

    memset(&stalarminfo, 0, sizeof(stalarminfo));
    Data_Get_Lock();
    memcpy(&stalarminfo, &g_Core_Data.stAlarmData, sizeof(stalarminfo));
    Data_Get_UnLock();

    result = rt_thread_init(&g_pm2_5_thread,
                            PM2_5_THREAD_NAME,
                            Pm2_5_thread_entry, RT_NULL,
                            &g_pm2_5_thread_stack[0], sizeof(g_pm2_5_thread_stack),
                            PM2_5_THREAD_PRIORITY, 10);

    if (result == RT_EOK) {
        rt_thread_startup(&g_pm2_5_thread);
    } else {
        Debug_Print(">>>rt_thread_init %s: failed\n", PM2_5_THREAD_NAME);
        return -1;
    }

    result = rt_thread_init(&g_time_switch_thread,
                            TIME_SWITCH_THREAD_NAME,
                            TimeSwitch_thread_entry, RT_NULL,
                            &g_time_switch_thread_stack[0], sizeof(g_time_switch_thread_stack),
                            TIME_SWITCH_THREAD_PRIORITY, 10);

    if (result == RT_EOK) {
        rt_thread_startup(&g_time_switch_thread);
    } else {
        Debug_Print(">>>rt_thread_init %s: failed\n", TIME_SWITCH_THREAD_NAME);
        return -1;
    }

    /* 初始化一个 mailbox */
    result = rt_mb_init(&tswitch_mb,
                        "tswitch_mb",                      /* 名称是 mbt */
                        &tswitch_mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                        sizeof(tswitch_mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        Debug_Print(">>>init mailbox failed.\n");
        return -1;
    }

    if (stalarminfo.stRtcTime.uiSyncFlag) {
        Debug_Print(">>>sync rtc time success.\n");
        Bsp_Rtc_Sync(stalarminfo.stRtcTime.uiYear, stalarminfo.stRtcTime.uiMon, stalarminfo.stRtcTime.uiDay, 
                    stalarminfo.stRtcTime.uiWeek, stalarminfo.stRtcTime.uiHour, 
                    stalarminfo.stRtcTime.uiMin, stalarminfo.stRtcTime.uiSec);
        Data_Get_Lock();
        g_Core_Data.stDevStatus.uiTimeSync = 1;
        Data_Get_UnLock();
    }

    return 0;
}

INIT_APP_EXPORT(Coredata_Pro_Init);
