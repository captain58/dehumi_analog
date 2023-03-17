#include "bsp_timer.h"
#include "../src/data.h"
#include "../src/iodev.h"

static uint32_t g_timer3_cnt = 0;
#define BSP_TIMER3_PERIOD_CNT       (24000)
#define BSP_PWM_INDEX_MAX           (2)

static uint16_t g_uiHighCnt[BSP_PWM_INDEX_MAX] = {0};        /* 高电平计数 */
static uint16_t g_uiLowCnt[BSP_PWM_INDEX_MAX] = {0};         /* 低电平计数 */
static uint16_t g_uiHighTime[BSP_PWM_INDEX_MAX] = {0};      /* 高电平定时时间，单位ms */
static uint16_t g_uiLowTime[BSP_PWM_INDEX_MAX] = {0};       /* 低电平定时时间，单位ms */
static uint8_t g_uiPwmStatus[BSP_PWM_INDEX_MAX] = {0};       /* pwm状态，0 关闭， 1高，2低 */

static uint8_t g_uiLedStatus = 0;
/*******************************************************************************
 * TIM3中断服务函数
 ******************************************************************************/
void Tim3_IRQHandler(void)
{
    rt_interrupt_enter();
    //Timer3 模式0 计时溢出中断
    if(TRUE == Tim3_GetIntFlag(Tim3UevIrq))
    {
        g_timer3_cnt++;
        if ((g_timer3_cnt % 1000) == 0) {
            if (g_uiLedStatus) {
                g_uiLedStatus = 0;
            } else {
                g_uiLedStatus = 1;
            }
            IoDevSetLED(g_uiLedStatus);
        }
        for (int i = 0; i < BSP_PWM_INDEX_MAX; i++) {
            if (g_uiPwmStatus[i] == 1) {
                g_uiHighCnt[i]++;
                if (g_uiHighCnt[i] >= g_uiHighTime[i]) {
                    Bsp_Pwm_SetStatus(i, 0);
                    g_uiHighCnt[i] = 0;
                    g_uiPwmStatus[i] = 2;
                }
            } else if (g_uiPwmStatus[i] == 2) {
                g_uiLowCnt[i]++;
                if (g_uiLowCnt[i] >= g_uiLowTime[i]) {
                    Bsp_Pwm_SetStatus(i, 1);
                    g_uiLowCnt[i] = 0;
                    g_uiPwmStatus[i] = 1;
                }
            }
        }
        
        Tim3_ClearIntFlag(Tim3UevIrq);  //Timer3模式0 中断标志清除
    }
    rt_interrupt_leave();
}

//Timer3 配置
void App_Timer3Cfg(uint16_t u16Period)
{
    uint16_t                    u16ArrValue;
    uint16_t                    u16CntValue;
    stc_tim3_mode0_cfg_t     stcTim3BaseCfg;
    
    //结构体初始化清零
    DDL_ZERO_STRUCT(stcTim3BaseCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralTim3, TRUE); //Base Timer外设时钟使能
    
    stcTim3BaseCfg.enWorkMode = Tim3WorkMode0;              //定时器模式
    stcTim3BaseCfg.enCT       = Tim3Timer;                  //定时器功能，计数时钟为内部PCLK
    stcTim3BaseCfg.enPRS      = Tim3PCLKDiv1;              //PCLK/16
    stcTim3BaseCfg.enCntMode  = Tim316bitArrMode;           //自动重载16位计数器/定时器
    stcTim3BaseCfg.bEnTog     = FALSE;
    stcTim3BaseCfg.bEnGate    = FALSE;
    stcTim3BaseCfg.enGateP    = Tim3GatePositive;
    
    Tim3_Mode0_Init(&stcTim3BaseCfg);                       //TIM3 的模式0功能初始化
        
    u16ArrValue = 0x10000 - u16Period ;
    
    Tim3_M0_ARRSet(u16ArrValue);                            //设置重载值(ARR = 0x10000 - 周期)
    
    u16CntValue = 0x10000 - u16Period;
    
    Tim3_M0_Cnt16Set(u16CntValue);                          //设置计数初值
    
    Tim3_ClearIntFlag(Tim3UevIrq);                          //清中断标志
    Tim3_Mode0_EnableIrq();                                 //使能TIM3中断(模式0时只有一个中断)
    EnableNvic(TIM3_IRQn, IrqLevel3, TRUE);                 //TIM3 开中断 
}

/* 设置pwm状态， index 索引   flag 0关1开 */
void Bsp_Pwm_SetStatus(uint8_t uiIndex, uint8_t uiFlag) {
    if (uiIndex == BSP_PWM_FENGSHAN) {
        if (uiFlag) {
            Bsp_SetGpioStatus(BSP_PWM_GPIO, BSP_PWM_PIN, 1);
        } else {
            Bsp_SetGpioStatus(BSP_PWM_GPIO, BSP_PWM_PIN, 0);
        }
    } else if (uiIndex == BSP_PWM_WATERPUMP) {
        if (uiFlag) {
            Bsp_SetGpioStatus(WATER_PUMP_GPIO, WATER_PUMP_PIN, 1);
        } else {
            Bsp_SetGpioStatus(WATER_PUMP_GPIO, WATER_PUMP_PIN, 0);
        }
    }
}

int Bsp_Timer_Init(void) {
    App_Timer3Cfg(BSP_TIMER3_PERIOD_CNT); //Timer3 配置; 1分频,周期24000-->24000*(1/24M) * 1 = 1000us = 1ms
    
    Bsp_Gpio_OutPutInit(BSP_PWM_GPIO, BSP_PWM_PIN, BSP_GPIO_PULL_UP_E, 1);
    Bsp_Gpio_OutPutInit(WATER_PUMP_GPIO, WATER_PUMP_PIN, BSP_GPIO_PULL_DOWN_E, 1);
    for (int i = 0; i < BSP_PWM_INDEX_MAX; i++) {
        Bsp_Timer_Pwm_SetPara(i, 50, 50);
        Bsp_Timer_PwmEnable(i, 0);
    }
    
    Tim3_M0_Run();   //TIM3 运行。
    return 0;
}

uint32_t Bsp_Timer3_GetMsCnt(void) {
    return g_timer3_cnt;
}

uint32_t Bsp_Timer3_GetUsCnt(void) {
    uint32_t u16CntValue = 0;
    u16CntValue = Tim3_M0_Cnt16Get() + BSP_TIMER3_PERIOD_CNT - 0x10000;
    return (u16CntValue * 1000 / BSP_TIMER3_PERIOD_CNT);
}

uint32_t Bsp_Timer_Sub(uint32_t time1, uint32_t time2) {
    if (time1 >= time2) {
        return (time1 - time2);
    } else {
        return (UINT32_MAX - time2 + time1);
    }
}

/* 设置pwm开关 ELEMAC_LEVEL_E */
void Bsp_Timer_PwmEnable(uint8_t uiIndex, uint8_t uiLevel) {
    if (uiIndex == BSP_PWM_FENGSHAN) {
        if (uiLevel == ELEMAC_LEVEL_CLOSE) {
            g_uiPwmStatus[uiIndex] = 0;
            Bsp_Pwm_SetStatus(uiIndex, 1);
        } else if (uiLevel == ELEMAC_LEVEL_LOW) {
            g_uiPwmStatus[uiIndex] = 1;
            Bsp_Timer_Pwm_SetPara(uiIndex, 90, 10);
        } else if (uiLevel == ELEMAC_LEVEL_MID) {
            g_uiPwmStatus[uiIndex] = 1;
            Bsp_Timer_Pwm_SetPara(uiIndex, 55, 45);
        } else if (uiLevel == ELEMAC_LEVEL_HIGH) {
            g_uiPwmStatus[uiIndex] = 1;
            Bsp_Timer_Pwm_SetPara(uiIndex, 20, 80);
        }
    } else if (uiIndex == BSP_PWM_WATERPUMP) {
#ifdef __USE_WATERDUMP_GPIO__
        if (uiLevel != WATER_DUMP_CLOSE) {
            g_uiPwmStatus[uiIndex] = 0;
            Bsp_Pwm_SetStatus(uiIndex, 1);
        }
#else
        if (uiLevel == WATER_DUMP_RUN) {
            g_uiPwmStatus[uiIndex] = 0;
            Bsp_Pwm_SetStatus(uiIndex, 1);
        } else if (uiLevel == WATER_DUMP_LOW) {
            Bsp_Timer_Pwm_SetPara(uiIndex, 50, 90);
            g_uiPwmStatus[uiIndex] = 1;
        } 
#endif
        if (uiLevel == WATER_DUMP_CLOSE) {
            g_uiPwmStatus[uiIndex] = 0;
            Bsp_Pwm_SetStatus(uiIndex, 0);
        }
    }
}

/* 设置pwm高低电平延时时间，单位ms */
void Bsp_Timer_Pwm_SetPara(uint8_t uiIndex, uint16_t uiHighTime, uint16_t uiLowTime) {
    if (uiIndex >= BSP_PWM_INDEX_MAX) {
        return;
    }

    g_uiHighTime[uiIndex] = uiHighTime;
    g_uiLowTime[uiIndex] = uiLowTime;
    return;
}

void rt_hw_us_delay(rt_uint32_t us)
{
    rt_uint32_t start, now, delta, reload, us_tick;
    start = SysTick->VAL;
    reload = SysTick->LOAD;
    us_tick = SystemCoreClock / 1000000UL;
    do {
        now = SysTick->VAL;
        delta = start > now ? start - now : reload + start - now;
    } while(delta < us_tick * us);
}

void delay1ms(uint32_t u32Cnt) {
    int i = 0;
    for (i = 0; i < u32Cnt; i++) {
        rt_hw_us_delay(1000);
    }
}

void delay100us(uint32_t u32Cnt) {
    int i = 0;
    for (i = 0; i < u32Cnt; i++) {
        rt_hw_us_delay(100);
    }
}

void delay10us(uint32_t u32Cnt) {
    int i = 0;
    for (i = 0; i < u32Cnt; i++) {
        rt_hw_us_delay(10);
    }
}
