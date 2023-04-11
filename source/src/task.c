#include "src/task.h"
static uint32_t g_dev_run_timecnt = 0;  /* 设备运行时间，单位s */
static uint32_t g_timecnt = 0;  /* 时间计数，单位s */
static uint32_t g_timecntcompressure = 0;  /* 压缩机压力时间计数，单位s */
static uint32_t g_com_run_time = 0;  /* 压缩机运行时间，单位s */

#define CON_HUMIDITY_MARGIN 300

/* 获取pm2.5的等级 1优2劣3良 */
uint8_t Get_Pm2_5_Level(uint16_t value) {
    if (value < 35) {
        return PM2_5_LEVEL_GOOD;
    } else {
        if (value < 75) {
            return PM2_5_LEVEL_GEN;
        } else {
            return PM2_5_LEVEL_BAD;
        }
    } 
}

/* 获取co2的等级 */
uint8_t Get_CO2_Level(uint16_t value) {
    return Get_Pm2_5_Level(value);
}

/* 获取甲醛的等级 */
uint8_t Get_formaldehyde_Level(uint16_t value) {
    return Get_Pm2_5_Level(value);
}

/* 模式运行设置 */
void Run_Mode_Set(uint8_t uiflag, DIGIT_STATUS_U *pstParaCmd, uint8_t uiResetFlag) {
    uint8_t uiResult = 0;
    RUN_MODE_E uimode = pstParaCmd->uiMode;

    if (uiflag == 1) {  /* 模式结束 */        
        Set_Dev_RunStatus(RUN_STATUS_STANDBY);
        Set_Dev_RunMode(RUN_MODE_STANDBY_E);
        g_dev_run_timecnt = 0;
        Data_Get_Lock();
        CORE_DATA_S *pstData = Data_Get_Point();
        pstData->stAlarmData.uiLastRunMode = 1;
        Data_Get_UnLock();

        /* 保存压缩机工作时间到flash */
        Store_Core_Data2Flash();
    } else {    /* 模式开启 */
        if (Get_Dev_RunStatus() != RUN_STATUS_STANDBY) {
            return;
        }

        Data_Get_Lock();
        CORE_DATA_S *pstData = Data_Get_Point();
        switch (uimode)
        {
            case RUN_MODE_AUTO_E:
                if (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityHigh) {
                    pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_DEHUMIDIFICATION_ING;
                    pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_STANDBY_E;
                    uiResult = 1;
                    Debug_fileline
                } else if (pstData->stInPutInfo.uiHumidity < pstData->stAlarmData.uiHumidityLow) {
                    pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_HUMIDIFICATION_ING;
                    pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                    uiResult = 1;
                    Debug_fileline
                } else {
                    pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_HUMIDIFICATION_ING;
                    pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                    uiResult = 1;
                    Debug_fileline
                }
                break;
            case RUN_MODE_DEHUMIDIFICATION_E:
                if (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityHigh) {
                    pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_DEHUMIDIFICATION_ING;
                    pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_STANDBY_E;
                    uiResult = 1;
                    Debug_fileline
                } else {
                    //if (uiResetFlag) 
                    {
                        pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_DEHUMIDIFICATION_ING;
                        pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_STANDBY_E;
                        uiResult = 1;
                        Debug_fileline
                    }
                    Debug_fileline
                }
                break;
            case RUN_MODE_HUMIDIFICATION_E:
                if (pstData->stInPutInfo.uiHumidity < pstData->stAlarmData.uiHumidityLow) {
                    pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_HUMIDIFICATION_ING;
                    pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                    uiResult = 1;
                    Debug_fileline
                } else {
                    //if (uiResetFlag) 
                    {
                        pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_HUMIDIFICATION_ING;
                        pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                        uiResult = 1;
                        Debug_fileline
                    }
                    Debug_fileline
                }
                break;
            case RUN_MODE_PURIFY_E:
                pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_PURIFY_ING;
                pstData->stDevStatus.uiDevRunSubStatus = PURIFY_STANDBY_E;
                uiResult = 1;
                Debug_fileline
                break;
            
            default:
                break;
        }
        Data_Get_UnLock();

        /* 成功开启模式 */
        if (uiResult) {
            Set_Dev_RunMode(uimode);
            /* 风扇必须打开,默认自动 */
            if (pstParaCmd->uiElectricMachinery == ELEMAC_LEVEL_CLOSE) {
                pstParaCmd->uiElectricMachinery = ELEMAC_LEVEL_AUTO;
            }
            Set_Dev_RunEnable(pstParaCmd);
            g_dev_run_timecnt = 0;
            Debug_Print("---->now start: mod:%u, com:%u, water:%u,uv:%u,ani:%u,ele:%u,ozone:%u,baiye:%u",
                        uimode, pstParaCmd->uiCompressor, pstParaCmd->uiWaterPump, pstParaCmd->uiUVLamp, pstParaCmd->uiAnion,
                        pstParaCmd->uiElectricMachinery, pstParaCmd->uiOzone, pstParaCmd->uiBaiYeFengJi);
            Data_Get_Lock();
            CORE_DATA_S *pstData = Data_Get_Point();
            pstData->stAlarmData.uiLastRunMode = 2;
            memcpy(&pstData->stAlarmData.stLastOutPutEnable, pstParaCmd, sizeof(DIGIT_STATUS_U));
            Data_Get_UnLock();

            /* 保存运行状态 */
            Store_Core_Data2Flash();
        }
    }
}

/* 设置部件运行状态,若参数为-1，表示默认状态，否则强制按此状态执行 iForce 为1时强制设置，否则相同状态不设置 */
void Task_Set_DeviceRun(int8_t iForce, int8_t iCompressor, int8_t iWaterPump, int8_t iUVLamp, int8_t iAnion, int8_t iElectricMachinery, int8_t iOzone, int8_t iBaiYe) {
    DIGIT_STATUS_U    stOutPutEnable;
    DIGIT_STATUS_U    stOutPutEnableTmp;

    /* 获取部件设置状态 */
    Get_Dev_RunEnable(&stOutPutEnable);
    /* 获取部件现在的状态 */
    Get_Dev_RunDiGit(&stOutPutEnableTmp);

    /* 压缩机状态 */
    if (iCompressor >= 0) {
        stOutPutEnable.uiCompressor = (uint8_t)iCompressor;
    }

    /* 加湿水泵状态 */
    if (iWaterPump >= 0) {
        stOutPutEnable.uiWaterPump = (uint8_t)iWaterPump;
    }

    /* 紫外灯状态 */
    if (iUVLamp >= 0) {
        stOutPutEnable.uiUVLamp = (uint8_t)iUVLamp;
    }

    /* 负离子状态 */
    if (iAnion >= 0) {
        stOutPutEnable.uiAnion = (uint8_t)iAnion;
    }

    /* 电机状态 */
    if (iElectricMachinery >= 0) {
        stOutPutEnable.uiElectricMachinery = (uint8_t)iElectricMachinery;
    }
    
    if (stOutPutEnable.uiElectricMachinery == ELEMAC_LEVEL_AUTO) {
        stOutPutEnable.uiElectricMachinery = ELEMAC_LEVEL_LOW;
    }

    /* 风机由低变高或者高变低时，必须先经过中档 */
    if (((stOutPutEnable.uiElectricMachinery == ELEMAC_LEVEL_LOW) && (stOutPutEnableTmp.uiElectricMachinery == ELEMAC_LEVEL_HIGH)) ||
        ((stOutPutEnable.uiElectricMachinery == ELEMAC_LEVEL_HIGH) && (stOutPutEnableTmp.uiElectricMachinery == ELEMAC_LEVEL_LOW))) {
        stOutPutEnable.uiElectricMachinery = ELEMAC_LEVEL_MID;
    }

    /* 臭氧状态 */
    if (iOzone >= 0) {
        stOutPutEnable.uiOzone = (uint8_t)iOzone;
    }

    /* 百叶风机状态 */
    if (iBaiYe >= 0) {
        stOutPutEnable.uiBaiYeFengJi = (uint8_t)iBaiYe;
    }

    /* 部件运行 */
    if ((iForce) || (stOutPutEnable.uiCompressor != stOutPutEnableTmp.uiCompressor)) {
        IoDevSetCompressor(stOutPutEnable.uiCompressor);
    }
    if ((iForce) || (stOutPutEnable.uiWaterPump != stOutPutEnableTmp.uiWaterPump)) {
        IoDevSetWaterPump(stOutPutEnable.uiWaterPump);
    }
    if ((iForce) || (stOutPutEnable.uiUVLamp != stOutPutEnableTmp.uiUVLamp)) {
        IoDevSetUVLamp(stOutPutEnable.uiUVLamp);
    }
    if ((iForce) || (stOutPutEnable.uiAnion != stOutPutEnableTmp.uiAnion)) {
        IoDevSetAnion(stOutPutEnable.uiAnion);
    }
    if ((iForce) || (stOutPutEnable.uiElectricMachinery != stOutPutEnableTmp.uiElectricMachinery)) {
        IoDevSetElectricMachinery(stOutPutEnable.uiElectricMachinery);
    }
    if ((iForce) || (stOutPutEnable.uiOzone != stOutPutEnableTmp.uiOzone)) {
        IoDevSetOzone(stOutPutEnable.uiOzone);
    }
    if ((iForce) || (stOutPutEnable.uiBaiYeFengJi != stOutPutEnableTmp.uiBaiYeFengJi)) {
        IoDevSetBaiYe(stOutPutEnable.uiBaiYeFengJi);
    }

    /* 保存部件当前状态 */
    Set_Dev_RunDiGit(&stOutPutEnable);
}

/* 获取自动档风机状态 1 除湿  2 加湿  3 净化 */
int8_t Get_Ele_LevelByHum(uint8_t uiFlag, uint16_t tm) {
    int8_t iLevel;

    CORE_DATA_S *pstData = NULL;

    Data_Get_Lock();
    pstData = Data_Get_Point();
    if (pstData->stOutPutEnable.uiElectricMachinery == ELEMAC_LEVEL_AUTO) {
        if (uiFlag == 1) {
            if (pstData->stInPutInfo.uiHumidity > (pstData->stAlarmData.uiHumidityHigh + 500)) {
                iLevel = ELEMAC_LEVEL_HIGH;
            } else {
                //if (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityHigh - CON_HUMIDITY_MARGIN || tm < 180) {
                iLevel = ELEMAC_LEVEL_MID;
                //} else {
                //    iLevel = ELEMAC_LEVEL_LOW;
                //}
            }
        } else if (uiFlag == 2) {
            if (pstData->stInPutInfo.uiHumidity < (pstData->stAlarmData.uiHumidityLow - 500)) {
                iLevel = ELEMAC_LEVEL_HIGH;
            } else {
                //if (pstData->stInPutInfo.uiHumidity < pstData->stAlarmData.uiHumidityLow) {
                    iLevel = ELEMAC_LEVEL_MID;
                //} 
                //else {
                //    iLevel = ELEMAC_LEVEL_LOW;
                //}
            }
        } else if (uiFlag == 3) {
            if (Get_Pm2_5_Level(pstData->stInPutInfo.uiPm2_5) == PM2_5_LEVEL_GOOD) {
                iLevel = ELEMAC_LEVEL_LOW;
            } else if (Get_Pm2_5_Level(pstData->stInPutInfo.uiPm2_5) == PM2_5_LEVEL_GEN) {
                iLevel = ELEMAC_LEVEL_MID;
            } else if (Get_Pm2_5_Level(pstData->stInPutInfo.uiPm2_5) == PM2_5_LEVEL_BAD) {
                iLevel = ELEMAC_LEVEL_HIGH;
            }
        }
        if (iLevel != pstData->stOutPutDigit.uiElectricMachinery) {
            Debug_Print("level %u->%d, cur humi:%u, high:%u,low:%u", pstData->stOutPutDigit.uiElectricMachinery, iLevel, pstData->stInPutInfo.uiHumidity, pstData->stAlarmData.uiHumidityHigh, pstData->stAlarmData.uiHumidityLow);
        }
    } else {
        if (uiFlag == 1) {
            if(pstData->stOutPutEnable.uiElectricMachinery < ELEMAC_LEVEL_MID)
            {
                iLevel = ELEMAC_LEVEL_MID;
            }
            else
            {
                iLevel = pstData->stOutPutEnable.uiElectricMachinery;
            }
        }
        else
        {
            iLevel = pstData->stOutPutEnable.uiElectricMachinery;
        }
    }
    Data_Get_UnLock();
    return iLevel;
}

struct printrunstatus
{
    uint8_t type;
    char data[32];
};

const struct printrunstatus S_printrunstatus[] =  {
    {RUN_STATUS_STOP, "RUN_STATUS_STOP"},
    {RUN_STATUS_STANDBY, "RUN_STATUS_STANDBY"},
    {RUN_STATUS_DEHUMIDIFICATION_ING, "RUN_STATUS_DEHUMIDIFICATION_ING"},
    {RUN_STATUS_HUMIDIFICATION_ING, "RUN_STATUS_HUMIDIFICATION_ING"},
    {RUN_STATUS_DEFROST_ING, "RUN_STATUS_DEFROST_ING"},
    {RUN_STATUS_PURIFY_ING, "RUN_STATUS_PURIFY_ING"},
};

const struct printrunstatus S_printrunmode[] =  {
    {RUN_MODE_STANDBY_E, "RUN_MODE_STANDBY_E"},
    {RUN_MODE_AUTO_E, "RUN_MODE_AUTO_E"},
    {RUN_MODE_DEHUMIDIFICATION_E, "RUN_MODE_DEHUMIDIFICATION_E"},
    {RUN_MODE_HUMIDIFICATION_E, "RUN_MODE_HUMIDIFICATION_E"},
    {RUN_MODE_PURIFY_E, "RUN_MODE_PURIFY_E"},
};

const struct printrunstatus S_printrunsubstatus[] =  {
    {DEHUMIDIFICATION_STANDBY_E, "DEHUMIDIFICATION_STANDBY_E"},
    {DEHUMIDIFICATION_WAIT_E, "DEHUMIDIFICATION_WAIT_E"},
    {DEHUMIDIFICATION_RUN_E, "DEHUMIDIFICATION_RUN_E"},
    {DEHUMIDIFICATION_DEFROST_E, "DEHUMIDIFICATION_DEFROST_E"},
    {HUMIDIFICATION_STANDBY_E, "HUMIDIFICATION_STANDBY_E"},
    {HUMIDIFICATION_RUN_E, "HUMIDIFICATION_RUN_E"},
    {PURIFY_STANDBY_E, "PURIFY_STANDBY_E"},
    {PURIFY_RUN_E, "PURIFY_RUN_E"},
    {PURIFY_BUTT_E, "PURIFY_BUTT_E"},
};

void Debug_DevCurMode_(uint8_t enRunMode) {
    rt_kprintf("\ncur mode:");
    for (size_t i = 0; i < ARRAY_SZ(S_printrunmode); i++) {
        if (S_printrunmode[i].type == enRunMode) {
            rt_kprintf("%s ", S_printrunmode[i].data);
        }
    }
}

void Debug_DevCurStatus_(uint8_t enRunStatus) {
    rt_kprintf("\ncur status:");
    for (size_t i = 0; i < ARRAY_SZ(S_printrunstatus); i++) {
        if (S_printrunstatus[i].type == enRunStatus) {
            rt_kprintf("%s ", S_printrunstatus[i].data);
        }
    }
}

void Debug_DevCurSubStatus_(uint8_t uiSubStatus) {
    rt_kprintf("\nsub status:");
    for (size_t i = 0; i < ARRAY_SZ(S_printrunsubstatus); i++) {
        if (S_printrunsubstatus[i].type == uiSubStatus) {
            rt_kprintf("%s\n", S_printrunsubstatus[i].data);
        }
    }
    return;
}
#define CON_HUMI_FILTER 10
void Task_thread_entry(void *parameter) {
    int8_t level;
    RUN_STATUS_E enRunStatus;
    RUN_MODE_E enRunMode;
    uint8_t uiSubStatus;
    CORE_DATA_S *pstData = NULL;
    uint32_t uiLoop = 0;
    static uint16_t s_2high_count=0,s_2mid_count=0,s_2low_count=0;
    /* 初始化关闭所有部件 */
    Task_Set_DeviceRun(1, 0, WATER_DUMP_CLOSE, 0, 0, ELEMAC_LEVEL_CLOSE, 0, 0);

    while (TRUE) {
        uiLoop++;
        enRunStatus = Get_Dev_RunStatus();
        enRunMode = Get_Dev_RunMode();
        uiSubStatus = Get_Dev_RunSubStatus();
        if ((uiLoop % 30) == 0) {
            pstData = Data_Get_Point();

            Debug_Print("-------> humi[%d] high=%d low=%d\n",pstData->stInPutInfo.uiHumidity, 
                pstData->stAlarmData.uiHumidityHigh, pstData->stAlarmData.uiHumidityLow);
            Debug_DevCurMode_(enRunMode);
            Debug_DevCurStatus_(enRunStatus);
            Debug_DevCurSubStatus_(uiSubStatus);
        }
        switch (enRunStatus)
        {
            case RUN_STATUS_STOP:
                /* 关闭所有部件 */
                Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, 0, 0, ELEMAC_LEVEL_CLOSE, 0, 0);
                break;
            case RUN_STATUS_STANDBY:
                /* 关闭所有部件 */
                Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, 0, 0, ELEMAC_LEVEL_CLOSE, 0, 0);
                break;
            case RUN_STATUS_DEHUMIDIFICATION_ING:
                /* 温湿度告警，管盘温度告警 暂停 */
                Data_Get_Lock();
                pstData = Data_Get_Point();
                if ((pstData->stDevStatus.uiTemperatureStatus == 1) || (pstData->stDevStatus.uiHumidityStatus == 1) || 
                    (pstData->stDevStatus.uiTubeStatus == 1) ) {
                    g_dev_run_timecnt = 0;
                    pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_STANDBY_E;
                    level = ELEMAC_LEVEL_CLOSE;
                    Debug_fileline
                }
                if(pstData->stInPutInfo.uiUpperWaterLevel == 1)
                {
                    g_dev_run_timecnt = 0;
                    pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_WAIT_E;
                    level = ELEMAC_LEVEL_LOW;
                    Debug_fileline
                }
                Data_Get_UnLock();
                switch (uiSubStatus)
                {
                    case DEHUMIDIFICATION_STANDBY_E:
                        /* 暂停状态，关闭压缩机，关闭风扇 */
                        Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, -1, -1, level, -1, -1);
                        if (g_dev_run_timecnt) {
                            Debug_fileline
                            Set_Dev_RunSubStatus(DEHUMIDIFICATION_WAIT_E);
                            g_dev_run_timecnt = 0;
                        }
                        break;
                    case DEHUMIDIFICATION_WAIT_E:
                        /* 进入等待模式，关闭压缩机，风机低速 */
                        Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, -1, -1, ELEMAC_LEVEL_LOW, -1, -1);
                        Data_Get_Lock();
                        pstData = Data_Get_Point();

                        /* 判断当前湿度,大于上限，启动除湿 */
                        
                        Debug_Print("DEHUMIDIFICATION_WAIT_E  cur[%d] level[%d]\n", 
                            pstData->stInPutInfo.uiHumidity, pstData->stAlarmData.uiHumidityHigh);
                        if (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityHigh) {
                            /* 压缩机工作间隔3分钟,且至少等待30s */

                            if (((rt_tick_get()/1000) - pstData->stAlarmData.iLastStopTime > 3*60) &&
                                (g_dev_run_timecnt > 30)) 
                            {
                                s_2high_count++;
                                s_2mid_count = 0;
                                s_2low_count = 0;
                                if(s_2high_count > CON_HUMI_FILTER)
                                {
                                    s_2high_count = 0;                                
                                    g_dev_run_timecnt = 0;
                                    pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_RUN_E;
                                    g_com_run_time = 1;
                                    g_timecnt = 0;      /* 管盘温度连续小于0°计数清零 */
                                    Debug_fileline
                                }
                            }
                        } 
                        else 
                        {
                            s_2high_count = 0;
                            s_2mid_count = 0;                            
                            /* 自动模式，且湿度小于设定下限，进入加湿模式 */
                            if ((enRunMode == RUN_MODE_AUTO_E) && (pstData->stInPutInfo.uiHumidity < pstData->stAlarmData.uiHumidityLow)) 
                            {
                                s_2low_count++;
                                

                                if(s_2low_count > CON_HUMI_FILTER)
                                {
                                    s_2low_count = 0;
                                
                                    g_dev_run_timecnt = 0;
                                    pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_HUMIDIFICATION_ING;
                                    pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                                    Debug_fileline
                                }
                            }
                        }
                        Data_Get_UnLock();
                        break;
                    case DEHUMIDIFICATION_RUN_E:
                        Data_Get_Lock();
                        pstData = Data_Get_Point();
                        pstData->stAlarmData.iLastStopTime = rt_tick_get()/1000;   /* 记录压缩机停机时间 */

                        #if 0
                        /* 压缩机工作时间大于20分钟后，判断 环境温度-管盘温度 若小于3°，表明压缩机工作出问题 */
                        if ((g_dev_run_timecnt > 20 * 60) && 
                            (pstData->stInPutInfo.iTemperature - pstData->stInPutInfo.iTubeTemperature < 300)) {
                            g_timecntcompressure++;
                        } else {
                            pstData->stDevStatus.uiComPreasure = 0;
                            g_timecntcompressure = 0;
                        }

                        /* 连续1分钟，表示压缩机有问题 */
                        if (g_timecntcompressure > 60) {
                            pstData->stDevStatus.uiComPreasure = 1;
                        }
                        #endif
                        /* 压缩机工作时间大于20分钟再判断管盘温度,且判断需连续 3分钟低于化霜启动温度 */
                        if ((g_dev_run_timecnt > 20 * 60)  && 
                            (pstData->stInPutInfo.iTubeTemperature < pstData->stAlarmData.iFrostingStartThr) &&
                            (pstData->stInPutInfo.iTubeTemperature < 0)) {
                            g_timecnt++;
                        } else {
                            g_timecnt = 0;
                        }

                        /* 连续3分钟处于化霜区间，开始化霜 */
                        if (g_timecnt > 3 * 60) {
                            g_dev_run_timecnt = 0;
                            pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_DEFROST_E;
                            Data_Get_UnLock();
                            g_timecnt = 0;
                            Debug_fileline
                            continue;
                        }

                        /* 工作时间大于3分钟，若湿度低于上限，切回等待模式 */
                        if ((g_dev_run_timecnt > 3 * 60) && (pstData->stInPutInfo.uiHumidity < pstData->stAlarmData.uiHumidityHigh - CON_HUMIDITY_MARGIN)) 
                        {
                            s_2low_count++;
                            s_2high_count = 0;
                            s_2mid_count = 0;                            
                            if(s_2low_count > CON_HUMI_FILTER)
                            {
                                s_2low_count = 0;
                                g_dev_run_timecnt = 0;
                                pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_WAIT_E;
                                Debug_fileline
                            }
                        }
                        else
                        {
                            s_2low_count = 0;
                        }
                        Data_Get_UnLock();

                        /* 除湿模式，开启压缩机 */
                        level = Get_Ele_LevelByHum(1, g_dev_run_timecnt);
                        Task_Set_DeviceRun(0, 1, WATER_DUMP_CLOSE, -1, -1, level, -1, -1);
                        g_com_run_time++;
                        break;
                    case DEHUMIDIFICATION_DEFROST_E:
                        Data_Get_Lock();
                        pstData = Data_Get_Point();
                        /* 
                            1,连续3分钟超过化霜结束时间，退出化霜
                            2，超时后判断管盘温度，>0 退出化霜，否则继续等待下一个超时时间
                         */
                        if (pstData->stInPutInfo.iTubeTemperature > pstData->stAlarmData.iFrostingEndThr) {
                            g_timecnt++;
                        } else {
                            g_timecnt = 0;
                        }

                        /* 连续3分钟超过化霜结束温度，退出 */
                        if (g_timecnt > 3 * 60) {
                            g_dev_run_timecnt = 0;
                            pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_WAIT_E;
                            Debug_fileline
                        }

                        /* 超过化霜时间，且温度大于0°，退出，否则继续下一个化霜时间 */
                        if (g_dev_run_timecnt > (pstData->stAlarmData.uiFrostingTime * 60)) {
                            if (pstData->stInPutInfo.iTubeTemperature > 0) {
                                pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_WAIT_E;
                                Debug_fileline
                            }

                            g_dev_run_timecnt = 0;
                        }
                        Data_Get_UnLock();

                        /* 化霜模式，关闭压缩机，风机自动 */
                        Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, -1, -1, ELEMAC_LEVEL_HIGH, -1, -1);
                        break;
                    default:
                        break;
                }
                break;
            case RUN_STATUS_HUMIDIFICATION_ING:
                Data_Get_Lock();
                pstData = Data_Get_Point();
                /* 下水位告警 或湿度告警 暂停 */
                if ((pstData->stInPutInfo.uiLowerWaterLevel) || (pstData->stDevStatus.uiHumidityStatus == 1))  {
                    g_dev_run_timecnt = 0;
                    pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                    Debug_fileline
                }

                /* 自动模式，且湿度大于设定上限，进入除湿模式 */
                if ((enRunMode == RUN_MODE_AUTO_E) && (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityHigh)) {
                    s_2high_count ++;
                    s_2low_count = 0;    
                    if(s_2high_count > CON_HUMI_FILTER)
                    {
                        s_2mid_count = 0;
                        g_dev_run_timecnt = 0;
                        pstData->stDevStatus.uiDevRunStatus = RUN_STATUS_DEHUMIDIFICATION_ING;
                        pstData->stDevStatus.uiDevRunSubStatus = DEHUMIDIFICATION_WAIT_E;
                        Debug_fileline
                    }
                }
                else
                {
                    s_2high_count = 0;
                }
                Data_Get_UnLock();
                switch (uiSubStatus)
                {
                    case HUMIDIFICATION_STANDBY_E:
                        /* 暂停状态，关闭水泵，风扇低速30s后打开水泵 */
                        Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, -1, -1, ELEMAC_LEVEL_LOW, -1, -1);
                    
                        /* 当前湿度比下限高 暂停 */
                        if (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityLow) {
                            s_2mid_count++;
                            s_2low_count = 0;    
                            if(s_2mid_count > CON_HUMI_FILTER)   
                            {
                                g_dev_run_timecnt = 0;
                                pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                                Debug_fileline
                            }
                        }
                        else
                        {
                            s_2mid_count = 0;
                        }
                    
                        if (g_dev_run_timecnt > 30) {
                            Set_Dev_RunSubStatus(HUMIDIFICATION_RUN_E);
                            g_timecnt = 0;
                            g_dev_run_timecnt = 0;
                            Debug_fileline
                        }
                        break;
                    case HUMIDIFICATION_RUN_E:
                        /* 加湿状态，打开加湿水泵 */
                    
                        /* 当前湿度比下限高 暂停 */
                        if (pstData->stInPutInfo.uiHumidity > pstData->stAlarmData.uiHumidityLow + CON_HUMIDITY_MARGIN) {
                            s_2mid_count++;
                            s_2low_count = 0;    
                            if(s_2mid_count > CON_HUMI_FILTER)   
                            {
                                g_dev_run_timecnt = 0;
                                pstData->stDevStatus.uiDevRunSubStatus = HUMIDIFICATION_STANDBY_E;
                                Debug_fileline
                            }
                        }
                        else
                        {
                            s_2mid_count = 0;
                        }                    
                    
                        level = Get_Ele_LevelByHum(2, g_dev_run_timecnt);
                        if (g_timecnt >= 120) {
                            Task_Set_DeviceRun(0, 0, WATER_DUMP_LOW, -1, -1, level, -1, -1);
                        } else {
                            Task_Set_DeviceRun(0, 0, WATER_DUMP_RUN, -1, -1, level, -1, -1);
                        }
                        g_timecnt++;
                        break;
                    default:
                        break;
                }
                break;
            case RUN_STATUS_PURIFY_ING:
                switch (uiSubStatus)
                {
                    case PURIFY_STANDBY_E:
                        if (g_dev_run_timecnt) {
                            Set_Dev_RunSubStatus(PURIFY_RUN_E);
                            g_dev_run_timecnt = 0;
                            Debug_fileline
                        }
                        break;
                    case PURIFY_RUN_E:
                        /* 打开净化部件 */
                        level = Get_Ele_LevelByHum(3, g_dev_run_timecnt);
                        Task_Set_DeviceRun(0, 0, WATER_DUMP_CLOSE, -1, -1, level, -1, -1);
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }

        g_dev_run_timecnt++;
        rt_thread_mdelay(1000);
    }
    
}

#define TASK_THREAD_STACK_SIZE            (512)
#define TASK_THREAD_NAME                  "task_t"
#define TASK_THREAD_PRIORITY               (20)
static struct rt_thread g_task_thread;
static char g_task_thread_stack[TASK_THREAD_STACK_SIZE];

int Task_Pro_Init(void) {
    rt_err_t result = RT_EOK;
    result = rt_thread_init(&g_task_thread,
                            TASK_THREAD_NAME,
                            Task_thread_entry, RT_NULL,
                            &g_task_thread_stack[0], sizeof(g_task_thread_stack),
                            TASK_THREAD_PRIORITY, 10);

    if (result == RT_EOK) {
        rt_thread_startup(&g_task_thread);
    } else {
        Debug_Print(">>>rt_thread_init %s: failed\n", TASK_THREAD_NAME);
        return -1;
    }
    return 0;
}

INIT_APP_EXPORT(Task_Pro_Init);
