#include "src/rs485pro.h"
#include "src/data.h"
#include "src/task.h"
#include "bsp/bsp_rtc.h"

uint64_t g_Rs485OnlineLastTime = 0;

/* 协议添加转义 */
uint8_t Rs485_Pro_AddEscape(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    uint8_t uiIndex = 0;

    if (uiInlen < 5) {
        return 0;
    }

    pcOutData[uiIndex++] = pcInData[0];
    pcOutData[uiIndex++] = pcInData[1];
    for (int i = 2; i < uiInlen - 2; i++) {
        if (uiIndex >= uiOutlen - 4) {
            return 0;
        }
        pcOutData[uiIndex++] = pcInData[i];
        if ((pcInData[i] == 0x9d) || (pcInData[i] == 0xdd) || (pcInData[i] == 0x55)) {
            pcOutData[uiIndex++] = 0x55;
        }
    }
    pcOutData[uiIndex++] = pcInData[uiInlen-2];
    pcOutData[uiIndex++] = pcInData[uiInlen-1];
    
    return uiIndex;

}

/* 协议删除转义 */
uint8_t Rs485_Pro_DelEscape(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    uint8_t uiIndex = 0;

    if (uiInlen < 5) {
        return 0;
    }

    pcOutData[uiIndex++] = pcInData[0];
    pcOutData[uiIndex++] = pcInData[1];
    for (int i = 2; i < uiInlen - 2; i++) {
        if (uiIndex >= uiOutlen - 4) {
            return 0;
        }
        pcOutData[uiIndex++] = pcInData[i];
        if (((pcInData[i] == 0x9d) || (pcInData[i] == 0xdd) || (pcInData[i] == 0x55)) && (pcInData[i+1] == 0x55)) {
            i++;
        }
    }
    pcOutData[uiIndex++] = pcInData[uiInlen-2];
    pcOutData[uiIndex++] = pcInData[uiInlen-1];
    
    return uiIndex;
}

/* 协议获取crc */
uint8_t Rs485_Pro_Crc8Check(uint8_t *pcInData, uint8_t uiInlen) {
    uint8_t uiInit = pcInData[0];

    for (int i = 1; i < uiInlen; i++) {
        uiInit ^= pcInData[i];
    }

    return uiInit;
}

int32_t Rs485_Pro_GetFloatDataByU8(uint8_t *pcData) {
    union Fdata fdata;

    if ((pcData == NULL) || ((pcData + 1) == NULL) || ((pcData + 2) == NULL) || ((pcData + 3) == NULL)) {
        return 0;
    }

    fdata.szdata[0] = pcData[3];
    fdata.szdata[1] = pcData[2];
    fdata.szdata[2] = pcData[1];
    fdata.szdata[3] = pcData[0];

    return (int32_t)(fdata.fvalue * 100);
}

void Rs485_Pro_GetU8DataByFloat(uint8_t *pcData, int32_t ivalue) {
    union Fdata fdata;

    if ((pcData == NULL) || ((pcData + 1) == NULL) || ((pcData + 2) == NULL) || ((pcData + 3) == NULL)) {
        return;
    }

    fdata.fvalue = (float)ivalue / 100;
    pcData[0] = fdata.szdata[3];
    pcData[1] = fdata.szdata[2];
    pcData[2] = fdata.szdata[1];
    pcData[3] = fdata.szdata[0];

    return;
}

uint8_t Rs485_Pro_TaskCmd1(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    CORE_DATA_S *pstData = NULL;
    uint16_t uiTemp;
    uint16_t uiIndex;

    if (uiInlen < 40) {
        Debug_Print("Rs485_Pro_TaskCmd1 failed! uiInlen:%u", uiInlen);
        return 0;
    }

    /* 非待机模式，不能下发配置 */
    if (Get_Dev_RunStatus() != RUN_STATUS_STANDBY) {
        Debug_Print("Rs485_Pro_TaskCmd1 failed! device is running");
        return 0;
    }

    Data_Get_Lock();
    pstData = Data_Get_Point();
    uiIndex = 4;
    pstData->stAlarmData.iTemperatureCorrect = Rs485_Pro_GetFloatDataByU8(&pcInData[uiIndex]);
    uiIndex += 4;
    pstData->stAlarmData.iHumidityCorrect = Rs485_Pro_GetFloatDataByU8(&pcInData[uiIndex]);
    uiIndex += 4;
    pstData->stAlarmData.uiPm2_5Correct = pcInData[uiIndex++];
    pstData->stAlarmData.iTubeTemperCorrect = Rs485_Pro_GetFloatDataByU8(&pcInData[uiIndex]);
    uiIndex += 4;
    // Debug_Print("iTemperatureCorrect:%d, iHumidityCorrect:%d", 
    //             pstData->stAlarmData.iTemperatureCorrect, pstData->stAlarmData.iHumidityCorrect);
    // Debug_Print("uiPm2_5Correct:%u, iTubeTemperCorrect:%d", 
    //             pstData->stAlarmData.uiPm2_5Correct, pstData->stAlarmData.iTubeTemperCorrect);
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    if (uiTemp) {
        pstData->stAlarmData.uiHumidityLow = uiTemp;
    }
    uiIndex += 2;

    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    if (uiTemp) {
        pstData->stAlarmData.uiHumidityHigh = uiTemp;
    }
    uiIndex += 2;
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    if (uiTemp) {
        pstData->stAlarmData.uiDehumiTime = uiTemp;
    }
    uiIndex += 2;
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    if (uiTemp) {
        pstData->stAlarmData.uiDehumiTimeDelay = uiTemp;
    }
    uiIndex += 2;
    pstData->stAlarmData.iFrostingStartThr = Rs485_Pro_GetFloatDataByU8(&pcInData[uiIndex]);
    uiIndex += 4;
    pstData->stAlarmData.iFrostingEndThr = Rs485_Pro_GetFloatDataByU8(&pcInData[uiIndex]);
    uiIndex += 4;
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    uiIndex += 2;
    if (uiTemp) {
        pstData->stAlarmData.uiFrostingTime = uiTemp;
    }
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    uiIndex += 2;
    if (uiTemp) {
        pstData->stAlarmData.uiUpdateWaterTime = uiTemp;
    }
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    uiIndex += 2;
    if (uiTemp) {
        pstData->stAlarmData.uiFilterChangeTime = uiTemp;
    }
    uiTemp = (pcInData[uiIndex] << 8) | pcInData[uiIndex + 1];
    uiIndex += 2;
    if (uiTemp) {
        pstData->stAlarmData.uiwetfilmChangeTime = uiTemp;
    }
    Data_Get_UnLock();
    Debug_Print("Rs485_Pro_TaskCmd1 Success!");
    Store_Core_Data2Flash();
    return 0;
}

uint8_t Rs485_Pro_TaskCmd2(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    return 0;
}

uint8_t Rs485_Pro_TaskCmd3(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    CORE_DATA_S *pstData = NULL;
    uint8_t uiIndex = 0;

    if (uiOutlen < 40) {
        Debug_Print("Rs485_Pro_TaskCmd3 failed! uiOutlen:%u", uiOutlen);
        return 0;
    }

    pcOutData[uiIndex++] = 0x9d;
    pcOutData[uiIndex++] = 0x9d;
    pcOutData[uiIndex++] = 0;
    pcOutData[uiIndex++] = 0x3;

    uint8_t uiDisplayStatus = Get_DevStatusForDisplay();
    Data_Get_Lock();
    pstData = Data_Get_Point();
    pcOutData[uiIndex++] = uiDisplayStatus;
    /* 上报化霜状态 */
    if ((pstData->stDevStatus.uiDevRunStatus == RUN_STATUS_DEHUMIDIFICATION_ING) && 
        (pstData->stDevStatus.uiDevRunSubStatus == DEHUMIDIFICATION_DEFROST_E)) {
        pcOutData[uiIndex++] = 1;
    } else {
        pcOutData[uiIndex++] = 0;
    }
    Rs485_Pro_GetU8DataByFloat(&pcOutData[uiIndex], pstData->stInPutInfo.iTemperature);
    uiIndex += 4;
    uint16_t uiHumidity = pstData->stInPutInfo.uiHumidity;
    pcOutData[uiIndex++] = uiHumidity >> 8;
    pcOutData[uiIndex++] = uiHumidity & 0xff;
    Rs485_Pro_GetU8DataByFloat(&pcOutData[uiIndex], pstData->stInPutInfo.iTubeTemperature);
    uiIndex += 4;
    if (pstData->stAlarmData.uiPm2_5Correct == 0) {
        pcOutData[uiIndex++] = Get_Pm2_5_Level(pstData->stInPutInfo.uiPm2_5);
        pcOutData[uiIndex++] = Get_CO2_Level(pstData->stInPutInfo.uiPm2_5);
        pcOutData[uiIndex++] = Get_formaldehyde_Level(pstData->stInPutInfo.uiPm2_5);
    } else {
        pcOutData[uiIndex++] = pstData->stAlarmData.uiPm2_5Correct;
        pcOutData[uiIndex++] = pstData->stAlarmData.uiPm2_5Correct;
        pcOutData[uiIndex++] = pstData->stAlarmData.uiPm2_5Correct;
    }
    pcOutData[uiIndex++] = pstData->stOutPutDigit.uiCompressor;
    if (pstData->stOutPutDigit.uiWaterPump == WATER_DUMP_CLOSE) {
        pcOutData[uiIndex++] = 0;
    } else {
        pcOutData[uiIndex++] = 1;
    }
    pcOutData[uiIndex++] = pstData->stOutPutDigit.uiUVLamp;
    pcOutData[uiIndex++] = pstData->stOutPutDigit.uiAnion;
    pcOutData[uiIndex++] = pstData->stOutPutDigit.uiElectricMachinery;
    pcOutData[uiIndex++] = pstData->stOutPutDigit.uiOzone;
    pcOutData[uiIndex++] = pstData->stOutPutDigit.uiBaiYeFengJi;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiComPreasure;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiTubeStatus;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiTemperatureStatus;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiHumidityStatus;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiFanStatus;
    pcOutData[uiIndex++] = pstData->stInPutInfo.uiUpperWaterLevel;
    pcOutData[uiIndex++] = pstData->stInPutInfo.uiLowerWaterLevel;
    pcOutData[uiIndex++] = pstData->stInPutInfo.uiWaterLeakage;
    pcOutData[uiIndex++] = pstData->stInPutInfo.uiMidWaterLevel;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiFilterStatus;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiupdatewaterStatus;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiwetfilmStatus;
    pcOutData[uiIndex++] = pstData->stDevStatus.uiStoreData;
    pcOutData[uiIndex++] = pstData->stAlarmData.uiTimOpenCloseFlag;
//	    if (pstData->stDevStatus.uiStoreData) {
//	        pstData->stDevStatus.uiStoreData = 0;
//	    }
    Data_Get_UnLock();
    pcOutData[2] = uiIndex - 1;
    pcOutData[uiIndex++] = Rs485_Pro_Crc8Check(&pcOutData[2], pcOutData[2]-1);
    pcOutData[uiIndex++] = 0xdd;
    pcOutData[uiIndex++] = 0xdd;
    return uiIndex;
}

uint8_t Rs485_Pro_TaskCmd4(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    DIGIT_STATUS_U stParaCmd;
    uint8_t uiMode;
    uint8_t uiflag;

    if (uiInlen < 15) {
        Debug_Print("Rs485_Pro_TaskCmd4 failed! uiInlen:%u", uiInlen);
        return 0;
    }

    if (Get_Dev_TimRunStatus()) {
        Debug_Print("---->device is tim running!");
        return 0;
    }

    uiMode = pcInData[4];
    uiflag = pcInData[5];
    stParaCmd.uiMode = uiMode + 1;
    stParaCmd.uiCompressor = pcInData[6];
    stParaCmd.uiWaterPump = pcInData[7];
    stParaCmd.uiUVLamp = pcInData[8];
    stParaCmd.uiAnion = pcInData[9];
    stParaCmd.uiElectricMachinery = pcInData[10];
    stParaCmd.uiOzone = pcInData[11];
    stParaCmd.uiBaiYeFengJi = pcInData[12];

    if (uiflag) {
        Debug_Print("---->now stop device");
    } else {
        Debug_Print("---->now start: com:%u, water:%u,uv:%u,ani:%u,ele:%u,ozone:%u,baiye:%u",
                    stParaCmd.uiCompressor, stParaCmd.uiWaterPump, stParaCmd.uiUVLamp, stParaCmd.uiAnion,
                    stParaCmd.uiElectricMachinery, stParaCmd.uiOzone, stParaCmd.uiBaiYeFengJi);
    }
    Debug_DevCurMode_(stParaCmd.uiMode);

    Run_Mode_Set(uiflag, &stParaCmd, 0);
    return 0;
}

uint8_t Rs485_Pro_TaskCmd5(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    stc_rtc_time_t curtime;
    CORE_DATA_S *pstData = NULL;

    if (uiInlen < 15) {
        Debug_Print("Rs485_Pro_TaskCmd5 failed! uiInlen:%u", uiInlen);
        return 0;
    }

    if (pcInData[11] == 7) {
        pcInData[11] = 0;
    }
    StoreRtcTime(pcInData[5], pcInData[6], pcInData[7], pcInData[11], pcInData[8], pcInData[9], pcInData[10]);
    Bsp_Rtc_Sync(pcInData[5], pcInData[6], pcInData[7], pcInData[11], pcInData[8], pcInData[9], pcInData[10]);
    Bsp_Rtc_GetTime(&curtime);
    Debug_Print("set cur rtc time: %02x-%02x-%02x %02x:%02x:%02x (week:%02x)", 
                curtime.u8Year, curtime.u8Month, curtime.u8Day, curtime.u8Hour, curtime.u8Minute, curtime.u8Second, curtime.u8DayOfWeek);
    Data_Get_Lock();
    pstData = Data_Get_Point();
    pstData->stDevStatus.uiTimeSync = 1;
    Data_Get_UnLock();

    return 0;
}

uint8_t Rs485_Pro_TaskCmd6(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    uint8_t uiMode;
    uint8_t uiFlag;
    CORE_DATA_S *pstData = NULL;
    
    if (uiInlen < 7) {
        Debug_Print("Rs485_Pro_TaskCmd6 failed! uiInlen:%u", uiInlen);
        return 0;
    }


    Debug_Print("Set Paishui status: %u, %u", pcInData[4], pcInData[5]);
    Data_Get_Lock();
    pstData = Data_Get_Point();
    if (pcInData[4]) {
        pstData->stAlarmData.uiPaiShuiMode = 0;
        pstData->stDevStatus.uiPaiShuiStatus = 0;
    } else {
        pstData->stAlarmData.uiPaiShuiMode = 1;
        pstData->stDevStatus.uiPaiShuiStatus = pcInData[5];
    }
    Data_Get_UnLock();
    Store_Core_Data2Flash();
    return 0;
}

uint8_t Rs485_Pro_TaskCmd7(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    CORE_DATA_S *pstData = NULL;

    if (uiInlen < 9) {
        Debug_Print("Rs485_Pro_TaskCmd7 failed! uiInlen:%u", uiInlen);
        return 0;
    }

    Data_Get_Lock();
    pstData = Data_Get_Point();
    if (pcInData[4] == 0) { /* 滤网 */
        pstData->stAlarmData.uiFilterChangeTimeCnt = 0;
        pstData->stDevStatus.uiFilterStatus = pcInData[5];
    } else if (pcInData[4] == 1) { /* 换水 */
        pstData->stAlarmData.uiUpdateWaterTimeCnt = 0;
        pstData->stDevStatus.uiupdatewaterStatus = pcInData[5];
    } else if (pcInData[4] == 2) { /* 湿膜 */
        pstData->stAlarmData.uiwetfilmChangeTimeCnt = 0;
        pstData->stDevStatus.uiwetfilmStatus = pcInData[5];
    }
    Data_Get_UnLock();

    return 0;
}

uint8_t Rs485_Pro_TaskCmd8(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    CORE_DATA_S *pstData = NULL;
    DIGIT_STATUS_U stParaCmd;
    uint8_t uiMode;
    uint8_t uiflag;

    if (uiInlen < 35) {
        Debug_Print("Rs485_Pro_TaskCmd8 failed! uiInlen:%u", uiInlen);
        return 0;
    }

    /* 定时关闭 */
    if (pcInData[4] == 0) {
        /* 当前不处于定时状态，该指令无效 */
        if (Get_Dev_TimRunStatus() == 0) {
            Debug_Print("Invalid Cmd!");
            return 0;
        }

        Debug_Print("---->timopen end!")
        Set_Dev_TimRunStatus(0);
        Store_Core_Data2Flash();
        Run_Mode_Set(1, &stParaCmd, 0);
    } else {        /* 定时打开 */
        if (pcInData[5]) {    /* 循环定时 */
            Set_TimCycleType(1);
        } else {    /* 单次定时 */
            Set_TimCycleType(0);
        }

        for (int i = 0; i < 7; i++) {
            if (pcInData[i+6] == 0) {
                Set_TimTypeByWeek(i+1, TIM_OPENCLOSE_F1);
            } else if (pcInData[i+6] == 1) {
                Set_TimTypeByWeek(i+1, TIM_OPENCLOSE_F2);
            } else if (pcInData[i+6] == 2) {
                Set_TimTypeByWeek(i+1, TIM_OPENCLOSE_F3);
            } else if (pcInData[i+6] == 3) {
                Set_TimTypeByWeek(i+1, TIM_OPENCLOSE_F4);
            }
        }
        
        /* 若全为常关状态，那么不开启定时任务 */
        if (Check_TimIsClosed()) {
            Debug_Print("Invalid Tim Task!");
            return 0;
        }

        Data_Get_Lock();
        pstData = Data_Get_Point();
        pstData->stAlarmData.stDataTimeStart[0].uiHour = DEC2BCD(pcInData[13]);
        pstData->stAlarmData.stDataTimeStart[0].uiMin = DEC2BCD(pcInData[14]);
        pstData->stAlarmData.stDataTimeEnd[0].uiHour = DEC2BCD(pcInData[15]);
        pstData->stAlarmData.stDataTimeEnd[0].uiMin = DEC2BCD(pcInData[16]);
        pstData->stAlarmData.stDataTimeStart[1].uiHour = DEC2BCD(pcInData[17]);
        pstData->stAlarmData.stDataTimeStart[1].uiMin = DEC2BCD(pcInData[18]);
        pstData->stAlarmData.stDataTimeEnd[1].uiHour = DEC2BCD(pcInData[19]);
        pstData->stAlarmData.stDataTimeEnd[1].uiMin = DEC2BCD(pcInData[20]);
        pstData->stAlarmData.stDataTimeStart[2].uiHour = DEC2BCD(pcInData[21]);
        pstData->stAlarmData.stDataTimeStart[2].uiMin = DEC2BCD(pcInData[22]);
        pstData->stAlarmData.stDataTimeEnd[2].uiHour = DEC2BCD(pcInData[23]);
        pstData->stAlarmData.stDataTimeEnd[2].uiMin = DEC2BCD(pcInData[24]);
        uiMode = pcInData[25] + 1;
        pstData->stAlarmData.stOutPutEnable.uiMode = uiMode;
        pstData->stAlarmData.stOutPutEnable.uiCompressor = pcInData[26];
        pstData->stAlarmData.stOutPutEnable.uiWaterPump = pcInData[27];
        pstData->stAlarmData.stOutPutEnable.uiUVLamp = pcInData[28];
        pstData->stAlarmData.stOutPutEnable.uiAnion = pcInData[29];
        pstData->stAlarmData.stOutPutEnable.uiElectricMachinery = pcInData[30];
        pstData->stAlarmData.stOutPutEnable.uiOzone = pcInData[31];
        pstData->stAlarmData.stOutPutEnable.uiBaiYeFengJi = pcInData[32];
        Data_Get_UnLock();
        Store_Core_Data2Flash();

        /* 关闭当前运行的设备 */
        Run_Mode_Set(1, &stParaCmd, 0);
        rt_thread_mdelay(500);

        Debug_Print("---->timopen start!")
        Debug_DevCurMode_(uiMode);

        /* 打开定时任务 */
        Set_Dev_TimRunStatus(1);
    }

    return 0;
}
uint8_t Rs485_Pro_TaskCmd9(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    DIGIT_STATUS_U stParaCmd;
    uint8_t uiMode;
    uint8_t uiflag;

    if (uiInlen < 4) {
        Debug_Print("Rs485_Pro_TaskCmd9 failed! uiInlen:%u", uiInlen);
        return 0;
    }

//	    if (Get_Dev_TimRunStatus()) {
//	        Debug_Print("---->device is tim running!");
//	        return 0;
//	    }

    uiMode = pcInData[4];
    uiflag = pcInData[5];
//	    stParaCmd.uiMode = uiMode + 1;
//	    stParaCmd.uiCompressor = pcInData[6];
//	    stParaCmd.uiWaterPump = pcInData[7];
//	    stParaCmd.uiUVLamp = pcInData[8];
//	    stParaCmd.uiAnion = pcInData[9];
//	    stParaCmd.uiElectricMachinery = pcInData[10];
//	    stParaCmd.uiOzone = pcInData[11];
//	    stParaCmd.uiBaiYeFengJi = pcInData[12];
//	
//	    if (uiflag) {
//	        Debug_Print("---->now stop device");
//	    } else {
//	        Debug_Print("---->now start: com:%u, water:%u,uv:%u,ani:%u,ele:%u,ozone:%u,baiye:%u",
//	                    stParaCmd.uiCompressor, stParaCmd.uiWaterPump, stParaCmd.uiUVLamp, stParaCmd.uiAnion,
//	                    stParaCmd.uiElectricMachinery, stParaCmd.uiOzone, stParaCmd.uiBaiYeFengJi);
//	    }
//	    Debug_DevCurMode_(stParaCmd.uiMode);
//	
//	    Run_Mode_Set(uiflag, &stParaCmd, 0);
    CORE_DATA_S *pstData = NULL;

    pstData = Data_Get_Point();

    if (uiflag = 0xaa && pstData->stDevStatus.uiStoreData) {
        pstData->stDevStatus.uiStoreData = 0;
    }

    return 0;
}

/* 
    处理协议指令
    pcInData  协议接收内容
    uiInlen  协议接收长度
    pcOutData  返回数据内容
    uiOutlen   返回数据最大缓存
    return   返回数据实际长度
 */
uint8_t Rs485_Pro_Task(uint8_t *pcInData, uint8_t uiInlen, uint8_t *pcOutData, uint8_t uiOutlen) {
    uint8_t uiCmd;
    uint8_t uiResult = 0;

    if (uiInlen < 7) {
        return 0;
    }

    g_Rs485OnlineLastTime = rt_tick_get();

    uiCmd = pcInData[3];
    switch (uiCmd)
    {
        case 1:     /* 参数下发指令 */
            uiResult = Rs485_Pro_TaskCmd1(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        // case 2:     /* 参数获取指令 */
        //     uiResult = Rs485_Pro_TaskCmd2(pcInData, uiInlen, pcOutData, uiOutlen);
        //     break;
        case 3:     /* 状态查询指令 */
            uiResult = Rs485_Pro_TaskCmd3(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        case 4:     /* 运行设置指令 */
            uiResult = Rs485_Pro_TaskCmd4(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        case 5:     /* 日期下发 */
            uiResult = Rs485_Pro_TaskCmd5(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        case 6:     /* 排水设置 */
            uiResult = Rs485_Pro_TaskCmd6(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        case 7:     /* 清除标志位等操作 */
            uiResult = Rs485_Pro_TaskCmd7(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        case 8:     /* 定时运行设置 */
            uiResult = Rs485_Pro_TaskCmd8(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        case 9:
            uiResult = Rs485_Pro_TaskCmd9(pcInData, uiInlen, pcOutData, uiOutlen);
            break;
        default:
            break;
    }

    return uiResult;
}

boolean_t Check_485_Pro(uint8_t *pcData, uint8_t uilen, uint8_t *puiOutlen) {
    uint8_t uiStart = 0xff;
    uint8_t uiEnd = 0xff;

    /* 校验长度 */
    if (uilen < 7) {
        // PrintHexLog(pcData, uilen, "len err");
        return FALSE;
    }
    
    /* 校验包头 */
    for (int i = 0; i < uilen - 1; i++) {
        if ((pcData[i] == 0x9d) && (pcData[i+1] == 0x9d)) {
            uiStart = i;
            break;
        }
    }

    if (uiStart == 0xff) {
        Debug_Print("Check_485_Pro not start!\n");
        return FALSE;
    }

    /* 校验包尾 */
    for (int i = uiStart; i < uilen - 1 - uiStart; i++) {
        if ((pcData[i] == 0xdd) && (pcData[i+1] == 0xdd)) {
            uiEnd = i+1;
            break;
        }
    }
    
    if (uiEnd == 0xff) {
        Debug_Print("Check_485_Pro not end!\n");
        return FALSE;
    }

    /* 转义处理 */
    uint8_t szRecvData[64] = {0};
    uint8_t uiRecvLen = Rs485_Pro_DelEscape(&pcData[uiStart], uiEnd-uiStart+1, szRecvData, sizeof(szRecvData));

    if (uiRecvLen > uilen) {
        Debug_Print("Check_485_Pro Escape failed! data len %u but max is %u\n", uiRecvLen, uilen);
        return FALSE;
    }

    /* 校验长度 */
    if (uiRecvLen < szRecvData[2] + 4) {
        Debug_Print("Check_485_Pro failed! total len:%u, but len data:%u\n", uiRecvLen, szRecvData[2]);
        return FALSE;
    }

    /* 校验crc */
    uint8_t uiCrc = Rs485_Pro_Crc8Check(&szRecvData[2], uiRecvLen-5);
    if (szRecvData[uiRecvLen - 3] != uiCrc) {
        Debug_Print("Check_485_Pro failed! crc is 0x%02x, but right crc is 0x%02x\n", szRecvData[uiRecvLen - 3], uiCrc);
        return FALSE;
    }

    /* 不处理黏包现象 */
    memset(pcData, 0, uilen);
    *puiOutlen = uiRecvLen;
    memcpy(pcData, szRecvData, uiRecvLen);
    return TRUE;
}

void Rs485_thread_entry(void *parameter) {
    uint8_t szRecv[64] = {0};
    uint8_t uiIndex = 0;
    int iRecvData;
    uint8_t uiRecvDataLen;
    uint32_t uiRecvTick = 0;

    while (1)
    {
        iRecvData = rt_rs485_getchar();
        if (rt_tick_get() - uiRecvTick > 300) {
            memset(szRecv, 0, sizeof(szRecv));
            uiIndex = 0;
        }
        uiRecvTick = rt_tick_get();
        szRecv[uiIndex++] = (uint8_t)iRecvData;
        if (Check_485_Pro(szRecv, uiIndex, &uiRecvDataLen)) {
            /* 处理接收数据 */
            PrintHexLog(szRecv, uiRecvDataLen, "recv 485");
            uint8_t szSendData[64] = {0};
            uint8_t uiSendLen = Rs485_Pro_Task(szRecv, uiRecvDataLen, szSendData, sizeof(szSendData));
            if (uiSendLen) {
                PrintHexLog(szSendData, uiSendLen, "send 485");
                memset(szRecv, 0, sizeof(szRecv));
                uiRecvDataLen = Rs485_Pro_AddEscape(szSendData, uiSendLen, szRecv, sizeof(szRecv));
                if (uiRecvDataLen == 0) {
                    Debug_Print("Rs485_Pro_AddEscape failed!");
                    PrintHexLog(szSendData, uiSendLen, "failed send data");
                } else {
                    Bsp_Rs485_SendData((char *)szRecv, uiRecvDataLen);
                }
            }
            memset(szRecv, 0, sizeof(szRecv));
            uiIndex = 0;
        }

        if (uiIndex >= sizeof(szRecv) - 1) {
            memset(szRecv, 0, sizeof(szRecv));
            uiIndex = 0;
        }
    }
}

#define RST485_THREAD_STACK_SIZE            (768)
#define RST485_THREAD_NAME                  "485_t"
#define RST485_THREAD_PRIORITY               (20)
static struct rt_thread g_rst485_thread;
static char g_rst485_thread_stack[RST485_THREAD_STACK_SIZE];

int Rs485_Pro_Init(void) {
    rt_err_t result = RT_EOK;
    result = rt_thread_init(&g_rst485_thread,
                            RST485_THREAD_NAME,
                            Rs485_thread_entry, RT_NULL,
                            &g_rst485_thread_stack[0], sizeof(g_rst485_thread_stack),
                            RST485_THREAD_PRIORITY, 10);

    if (result == RT_EOK) {
        rt_thread_startup(&g_rst485_thread);
    } else {
        Debug_Print(">>>rt_thread_init %s: failed\n", RST485_THREAD_NAME);
        return -1;
    }
    return 0;
}

INIT_APP_EXPORT(Rs485_Pro_Init);
