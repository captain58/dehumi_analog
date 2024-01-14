#ifndef __SRC_DATA_H__
#define __SRC_DATA_H__

#include <rtthread.h>
#include "bsp/bsp_pm2_5.h"
#include "bsp/bsp_adc_ntc.h"
#include "src/iodev.h"
#include "main.h"
#include "bsp/bsp_flash.h"
#include "bsp/bsp_rtc.h"

typedef enum run_mode {
    RUN_MODE_STANDBY_E = 0,             /* 待机模式 */
    RUN_MODE_AUTO_E,                    /* 自动模式 */
    RUN_MODE_DEHUMIDIFICATION_E,        /* 除湿模式 */
    RUN_MODE_HUMIDIFICATION_E,          /* 加湿模式 */
    RUN_MODE_PURIFY_E,                  /* 净化模式 */
    RUN_MODE_STOP_E,                    /* 停机模式 */
}RUN_MODE_E;

typedef enum run_status {
    RUN_STATUS_STOP = 0,                    /* 停机 */
    RUN_STATUS_STANDBY,                     /* 待机 */
    RUN_STATUS_DEHUMIDIFICATION_ING,        /* 正在除湿 */
    RUN_STATUS_HUMIDIFICATION_ING,          /* 正在加湿 */
    RUN_STATUS_DEFROST_ING,                 /* 正在化霜 */
    RUN_STATUS_PURIFY_ING,                  /* 正在净化 */
    RUN_STATUS_DEHUMIDIFICATION_WAITING,    /* 等待除湿 */
    RUN_STATUS_HUMIDIFICATION_WAITING,      /* 等待加湿 */
}RUN_STATUS_E;

typedef enum run_show {
    RUN_SHOW_AUOT = 0,                    /* 自动 */
    RUN_SHOW_DEHUMIDIFICATION_ING,        /* 正在除湿 */
    RUN_SHOW_HUMIDIFICATION_ING,          /* 正在加湿 */
    RUN_SHOW_PURIFY_ING,                  /* 正在净化 */
    RUN_SHOW_DEFROST,             /* 正在化霜 */    
    RUN_SHOW_STANDBY,                     /* 待机 */
    RUN_SHOW_STOP,                        /* 停机 */

}RUN_SHOW_E;

/* 除湿状态子状态 */
typedef enum dehumidification_status {
    DEHUMIDIFICATION_STANDBY_E = 1,        /* 暂停模式 */
    DEHUMIDIFICATION_WAIT_E,                /* 除湿间隔模式 */
    DEHUMIDIFICATION_RUN_E,                 /* 除湿运行模式 */
    DEHUMIDIFICATION_DEFROST_E,              /* 化霜模式 */
    DEHUMIDIFICATION_BUTT_E
}DEHUMIDIFICATION_STATUS_E;

/* 加湿状态子状态 */
typedef enum humidification_status {
    HUMIDIFICATION_STANDBY_E = DEHUMIDIFICATION_BUTT_E,        /* 暂停模式 */
    HUMIDIFICATION_RUN_E,                 /* 加湿运行模式 */
    HUMIDIFICATION_BUTT_E             
}HUMIDIFICATION_STATUS_E;

/* 净化状态子状态 */
typedef enum purify_status {
    PURIFY_STANDBY_E = HUMIDIFICATION_BUTT_E,        /* 暂停模式 */
    PURIFY_RUN_E,                 /* 净化运行模式 */
    PURIFY_BUTT_E             
}PURIFY_STATUS_E;

/* 风机等级 */
typedef enum ElectricMachinery_level {
    ELEMAC_LEVEL_AUTO = 0,          /* 自动 */
    ELEMAC_LEVEL_LOW,           /* 低档 */
    ELEMAC_LEVEL_MID,           /* 中档 */
    ELEMAC_LEVEL_HIGH,          /* 高档 */
    ELEMAC_LEVEL_CLOSE,     /* 关闭 */
}ELEMAC_LEVEL_E;

/* 加湿泵等级 */
typedef enum WaterDump_level {
    WATER_DUMP_CLOSE = 0,     /* 关闭 */
    WATER_DUMP_RUN,          /* 全速 */
    WATER_DUMP_LOW,           /* 低速 */
}WATER_DUMP_E;

/* 时间日期 */
typedef struct DataTime
{
    uint8_t     uiHour;
    uint8_t     uiMin;
}DATA_TIME_S;

/* rtc时间日期 */
typedef struct RtcTime
{
    uint8_t     uiSyncFlag;
    uint8_t     uiYear;
    uint8_t     uiMon;
    uint8_t     uiDay;
    uint8_t     uiHour;
    uint8_t     uiMin;
    uint8_t     uiSec;
    uint8_t     uiWeek;
}RTC_TIME_S;

/* 部件开关 */
typedef struct Digit_Status
{
    uint8_t uiMode;                     /* 模式 */
    uint8_t uiCompressor;               /* 压缩机状态 */
    uint8_t uiWaterPump;                /* 加湿水泵状态 */
    uint8_t uiUVLamp;                   /* 紫外灯状态 */
    uint8_t uiAnion;                    /* 负离子状态 */
    uint8_t uiElectricMachinery;        /* 电机状态 */
    uint8_t uiOzone;                    /* 臭氧状态 */
    uint8_t uiBaiYeFengJi;              /* 百叶风机状态 */
}DIGIT_STATUS_U;

typedef enum TimOpenCloseType {
    TIM_OPENCLOSE_F4 = 0,       /* 常关 */
    TIM_OPENCLOSE_F3,           /* 常开 */
    TIM_OPENCLOSE_F2,           /* 2个时间段 */
    TIM_OPENCLOSE_F1,           /* 1个时间段 */
    TIM_OPENCLOSE_BUTT            /* 无效值 */
}TIM_OPENCLOSE_TYPE_E;

/* 告警阈值 */
typedef struct Alarm_Data
{
    uint8_t  uiPm2_5Correct;            /* 净化 修正值 */
    uint8_t  uiPaiShuiMode;             /* 排水模式，0 自动  1 手动 自动模式下排水到中水位为止。手动模式下发指令开关 */
    uint8_t  uiTimOpenCloseFlag;      /* 定时开关机标志 0关，1开  */
    uint8_t  uiLastRunMode;             /* 掉电保存上一次的开启关闭状态 1 关闭 2 打开 */
    uint16_t  uiTimOpenCloseMap;      /* 定时开关机标志图，周一占 0~1 2个bit，周二占 2~3两个bit 以此类推。2个bit表示 TIM_OPENCLOSE_TYPE_E 类型 第14个bit 0表示单次，1表示循环 */
    uint16_t uiHumidityHigh;                /* 湿度上限 除湿启动阈值 */
    uint16_t uiHumidityLow;                 /* 湿度下限 加湿启动阈值 */
    uint16_t uiDehumiTime;                    /* 压缩机 启动时间 单位分钟 */
    uint16_t uiDehumiTimeDelay;               /* 压缩机 间隔时间 单位分钟 */
    uint16_t uiFrostingTime;                /* 化霜时间 单位分钟 */
    uint16_t uiUpdateWaterTime;                /* 换水时间 单位天 */
    uint16_t  uiFilterChangeTime;            /* 滤网更换时间 单位天 */
    uint16_t  uiwetfilmChangeTime;            /* 湿膜更换时间 单位天 */
    int32_t  iTemperatureCorrect;           /* 温度 修正值 */
    int32_t  iTubeTemperCorrect;           /* 管盘温度 修正值 */
    int32_t  iHumidityCorrect;             /* 湿度 修正值 */
    int32_t  iFrostingStartThr;                  /* 化霜温度启动阈值 */
    int32_t  iFrostingEndThr;                  /* 化霜温度结束阈值 */
    int32_t  iLastStopTime;               /* 压缩机 最后一次停机时间 单位s */
    uint32_t  uiUpdateWaterTimeCnt;                /* 换水时间计数 单位分钟 */
    uint32_t  uiFilterChangeTimeCnt;            /* 滤网更换计数 单位分钟 */
    uint32_t  uiwetfilmChangeTimeCnt;            /* 湿膜更换计数 单位分钟 */
    DATA_TIME_S    stDataTimeStart[3];         /* 定时开始时间日期 */
    DATA_TIME_S    stDataTimeEnd[3];           /* 定时结束时间日期 */
    DIGIT_STATUS_U stOutPutEnable;          /* 定时开关的部件操作状态 */
    DIGIT_STATUS_U stLastOutPutEnable;          /* 掉电保存上一次的部件操作状态 */
    RTC_TIME_S      stRtcTime;              /* rtc 时间 */
}ALARM_DATA_S;

/* 采集输入数据 */
typedef struct Input_Info
{
    uint8_t uiWaterLeakage;             /* 漏水开关 */
    uint8_t uiMidWaterLevel;          /* 中水位开关 */
    uint8_t uiLowerWaterLevel;          /* 下水位开关 */
    uint8_t uiUpperWaterLevel;          /* 上水位开关 */
    uint8_t uiCompressorPressure;       /* 压缩机压力开关 */
    uint8_t ucFanChkStat;                  /* 风机状态*/    
    int32_t  iTemperature;              /* 温度 温湿度传感器 */
    uint16_t uiHumidity;                /* 湿度 温湿度传感器 */
    uint16_t uiPm2_5;                   /* PM2.5浓度 */
    int32_t  iTubeTemperature;          /* 管盘温度 热敏电阻 */


}INPUT_INFO_S;

/* 设备运行状态 */
typedef struct Device_Run_Status
{
    RUN_MODE_E uiDevRunMode;               /* 设备运行模式 */
    RUN_STATUS_E uiDevRunStatus;               /* 设备运行状态 */
    uint8_t uiDevRunSubStatus;          /* 设备运行子状态 */
    uint8_t uiPaiShuiStatus;            /* 排水状态，0关，1开。自动排水模式下一直关闭 */
    uint8_t uiPrintFlag;                /* 打印开关 */
    uint8_t uiTimingFlag;               /* 定时开关机 0关闭1开启 */
    uint8_t uiTubeStatus;               /* 管盘传感器状态 */
    uint8_t uiTemperatureStatus;        /* 温度状态 */
    uint8_t uiHumidityStatus;           /* 湿度状态 */
    uint8_t uiFanStatus;                /* 风扇状态 */
    uint8_t uiFilterStatus;             /* 滤网状态 */
    uint8_t uiupdatewaterStatus;             /* 换水状态 */
    uint8_t uiwetfilmStatus;             /* 湿膜状态 */
    uint8_t uiStoreData;                /* 保存操作记录 */
    uint8_t uiTimeSync;                 /* 时间同步标志 */
    uint8_t uiComPreasure;                 /* 压缩机压力告警标志 */
    uint32_t uiHighTemperCnt;    /* 高温计数 单位s */
    uint32_t uiStopTemperCnt;    /* 温湿度无法获取计数 单位s */

}DEVICE_RUN_STATUS_S;

typedef struct Core_Data
{
    struct rt_mutex         coredatametux;
    DEVICE_RUN_STATUS_S     stDevStatus;        /* 设备运行状态 */
    DIGIT_STATUS_U          stOutPutDigit;      /* 检测部件状态 */
    DIGIT_STATUS_U          stOutPutEnable;     /* 部件操作 */
    INPUT_INFO_S            stInPutInfo;        /* 采集输入数据 */    
    ALARM_DATA_S            stAlarmData;        /* 告警阈值 */
}CORE_DATA_S;

/* 低温停机温度 */
#define ENV_TEMPERATURE_STOP           (-500)

/* 高温停机温度 */
#define ENV_TEMPERATURE_STOP_HIGH           (4000)

/* 高温停机持续时间 单位s */
#define ENV_TEMPERATURE_STOP_HIGH_TIME           (60)

CORE_DATA_S *Data_Get_Point(void);

void Data_Get_Lock(void);

void Data_Get_UnLock(void);

/* 获取设备运行模式 */
RUN_MODE_E Get_Dev_RunMode(void);
/* 设置设备运行模式 */
void Set_Dev_RunMode(RUN_MODE_E uiMode);

/* 获取设备运行状态 */
RUN_STATUS_E Get_Dev_RunStatus(void);
/* 设置设备运行状态 */
void Set_Dev_RunStatus(RUN_STATUS_E uiStatus);

/* 获取设备运行子状态 */
uint8_t Get_Dev_RunSubStatus(void);

/* 设置设备运行子状态 */
void Set_Dev_RunSubStatus(uint8_t uiStatus);
/* 获取设备定时开关状态 */
uint8_t Get_Dev_TimRunStatus(void);
/* 设置设备定时开关状态   0关 1开 */
void Set_Dev_TimRunStatus(uint8_t uiFlag);

/* 获取设备部件使能状态 */
void Get_Dev_RunEnable(DIGIT_STATUS_U *pstEnable);
/* 设置设备部件使能状态 */
void Set_Dev_RunEnable(DIGIT_STATUS_U *pstEnable);
/* 设置部件当前状态 */
void Set_Dev_RunDiGit(DIGIT_STATUS_U *pstDiGit);
/* 获取部件当前状态 */
void Get_Dev_RunDiGit(DIGIT_STATUS_U *pstDiGit);
/* 获取风扇开关 */
uint8_t Get_Ele_Enable(void);

/* 保存核心数据 */
void Store_Core_Data2Flash(void);

/* 读取核心数据 */
void Get_Core_DataFromFlash(void);

/* 根据星期设置该天的定时类型 */
void Set_TimTypeByWeek(uint8_t uiWeek, TIM_OPENCLOSE_TYPE_E enType);

/* 设置当前是单次定时还是循环定时，0单次，1循环 */
void Set_TimCycleType(uint8_t uiFlag);

/* 判断定时是否结束（单次循环且所有模式都是常关）若关闭，返回1且存flash，否则返回0 */
uint8_t Check_TimIsClosed(void);

/* 根据星期获取该天的定时类型 */
TIM_OPENCLOSE_TYPE_E Get_TimTypeByWeek(uint8_t uiWeek);

/* 获取当前是单次定时还是循环定时，0单次，1循环 */
uint8_t Get_TimCycleType(void);

/* 获取设备显示运行状态 */
RUN_STATUS_E Get_DevStatusForDisplay(void);

void StoreRtcTime(uint8_t u8Year, uint8_t u8Month, uint8_t u8Day, uint8_t u8DayOfWeek, uint8_t u8Hour, uint8_t u8Minute, uint8_t u8Second);

#endif
