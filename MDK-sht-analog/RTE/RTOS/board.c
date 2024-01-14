/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2021-05-24                  the first version
 */

#include <rthw.h>
#include <rtthread.h>
#include "sysctrl.h"
#include "system_hc32l13x.h"
#include "bsp/bsp_uart.h"
#include "bsp/bsp_timer.h"
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
/*
 * Please modify RT_HEAP_SIZE if you enable RT_USING_HEAP
 * the RT_HEAP_SIZE max value = (sram size - ZI size), 1024 means 1024 bytes
 */
#define RT_HEAP_SIZE (15*1024)
static rt_uint8_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif
uint32_t g_ulSysTick = 0;
void rt_os_tick_callback(void)
{
    rt_interrupt_enter();
    
    rt_tick_increase();
    g_ulSysTick++;
    rt_interrupt_leave();
}

/* cortex-m 架构使用 SysTick_Handler() */
void SysTick_Handler()
{
    rt_os_tick_callback();
}

/**
 * This function will initial your board.
 */
void rt_hw_board_init(void)
{
// #error "TODO 1: OS Tick Configuration."
    /* 
     * TODO 1: OS Tick Configuration
     * Enable the hardware timer and call the rt_os_tick_callback function
     * periodically with the frequency RT_TICK_PER_SECOND. 
     */

    stc_sysctrl_clk_cfg_t stcCfg;

    ///< 时钟初始化前，优先设置要使用的时钟源：此处设置RCH为4MHz
    Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz);
    Sysctrl_SetRCLTrim(SysctrlRclFreq32768); //SysctrlRclFreq32768.SysctrlRclFreq38400
    Sysctrl_ClkSourceEnable(SysctrlClkRCL, TRUE);
    Sysctrl_SysClkSwitch(SysctrlClkRCL);    
    
    ///< rch  可以加载  8，16,22.12M 24M
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);
    Sysctrl_SysClkSwitch(SysctrlClkRCH);

    ///< 选择内部RCH作为HCLK时钟源;
    stcCfg.enClkSrc    = SysctrlClkRCH;
    ///< HCLK SYSCLK/1
    stcCfg.enHClkDiv   = SysctrlHclkDiv1;
    ///< PCLK 为HCLK/1
    stcCfg.enPClkDiv   = SysctrlPclkDiv1;
    ///< 系统时钟初始化
    Sysctrl_ClkInit(&stcCfg);

    /* timer3时钟初始化 */
    Bsp_Timer_Init();

    /// 对系统时钟更新
    SystemCoreClockUpdate();

    ///< 内核函数，SysTick配置 RT_TICK_PER_SECOND = 1000 表示 1ms 触发一次中断
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /* Call components board initial (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

#ifdef RT_USING_CONSOLE
static int uart_init(void)
{
// #error "TODO 2: Enable the hardware uart and config baudrate."
    return BspUartInit();
}
INIT_BOARD_EXPORT(uart_init);

void rt_hw_console_output(const char *str)
{
// #error "TODO 3: Output the string 'str' through the uart."
    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++)
    {
        if (*(str + i) == '\n')
        {
            Bsp_Uart_SendChar(0, a);
        }
        Bsp_Uart_SendChar(0, *(str + i));
    }
}

#endif

