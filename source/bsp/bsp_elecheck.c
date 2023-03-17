#include "bsp_elecheck.h"

static uint8_t g_uiChangeCnt = 0;

///< PortB中断服务函数
void PortB_IRQHandler(void)
{
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    g_uiChangeCnt++;
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}  

uint8_t Bsp_EleCheck_Sync(void) {
    uint8_t bIsRun = 0;

    /* 设置为输入并开启中断 */
    g_uiChangeCnt = 0;
    Bsp_Gpio_InPutInit(ELEMACHFG_GPIO, ELEMACHFG_PIN, BSP_GPIO_PULL_DISABLE_E);
    Gpio_EnableIrq(ELEMACHFG_GPIO, ELEMACHFG_PIN, GpioIrqRising);
    Gpio_EnableIrq(ELEMACHFG_GPIO, ELEMACHFG_PIN, GpioIrqFalling);
    EnableNvic(PORTB_IRQn, IrqLevel3, TRUE);

    /* 延时等待 */
    rt_thread_mdelay(500);

    if (g_uiChangeCnt > 0) {
        bIsRun = 1;
    }

    /* 关闭中断 */
    Gpio_DisableIrq(ELEMACHFG_GPIO, ELEMACHFG_PIN, GpioIrqRising);
    Gpio_DisableIrq(ELEMACHFG_GPIO, ELEMACHFG_PIN, GpioIrqFalling);

    return bIsRun;
}
