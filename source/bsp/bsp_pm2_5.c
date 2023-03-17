#include "bsp_pm2_5.h"

#define BSP_PM2_5_CHECK_TIME        (15)
static uint32_t g_pm2_5totalCnt = 0;
static uint32_t g_pm2_5StartCnt = 0;
static uint32_t g_pm2_5StartCnt1 = 0;

///< PortA中断服务函数
void PortA_IRQHandler(void)
{
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    Bsp_Pm2_5_IrqCb();
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}  

void Bsp_Pm2_5_IrqCb(void) {
    if (TRUE == Gpio_GetIrqStatus(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN)) {            
        if (TRUE == Bsp_GetGpioStatus(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN)) {
            g_pm2_5totalCnt += Bsp_Timer_Sub(Bsp_Timer3_GetMsCnt(), g_pm2_5StartCnt1);
        } else {
            g_pm2_5StartCnt1 = Bsp_Timer3_GetMsCnt();
        }
        Gpio_ClearIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN);    
    }
}

uint16_t Bsp_Pm2_5_Sync(void) {
    g_pm2_5StartCnt = Bsp_Timer3_GetMsCnt();
    g_pm2_5StartCnt1 = Bsp_Timer3_GetMsCnt();
    g_pm2_5totalCnt = 0;

    /* 设置为输入并开启中断 */
    Bsp_Gpio_InPutInit(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, BSP_GPIO_PULL_DISABLE_E);
    Gpio_EnableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqRising);
    Gpio_EnableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqFalling);
    EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);

    rt_thread_mdelay(BSP_PM2_5_CHECK_TIME * 1000);
    
    Gpio_DisableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqRising);
    Gpio_DisableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqFalling);
    uint16_t result = g_pm2_5totalCnt * 1000 / Bsp_Timer_Sub(Bsp_Timer3_GetMsCnt(), g_pm2_5StartCnt);
    return result;
}
