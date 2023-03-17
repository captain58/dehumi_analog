#include "bsp_dht11.h"
#include "bsp_pm2_5.h"
#include "main.h"

#ifdef __BPS_USE_DHT11_H__
/*
    开始采集时，设置初始化时间 g_szDht11InitTime 为当前ms数
    有中断触发时，记录当前时间   （ms - g_szDht11InitTime）*1000+us 到数组g_szDht11Time中
    若为上升沿（当前io为高） g_szDht11Time 最高位为1， 若为下降沿（当前io为低） 最高位为0
    因此，每次采集时间不能大于 32ms
    数据共40个字节，加上响应共82个升降，再加上开头下降沿以及最后上升沿，共84个时间
*/
#define BSP_DHT11_IRQTIME_MAXLEN        (84)
static uint16_t g_szDht11Time[BSP_DHT11_IRQTIME_MAXLEN];
static uint8_t g_uiTimeIndex;
static uint32_t g_szDht11InitTime;

///< PortB中断服务函数
void PortB_IRQHandler(void)
{
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    Bsp_Dht11_IrqCb();
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}  

void Bsp_Dht11_IrqCb(void) {
    if ((TRUE == Gpio_GetIrqStatus(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN)) && (g_uiTimeIndex < BSP_DHT11_IRQTIME_MAXLEN)) {            
        g_szDht11Time[g_uiTimeIndex] = Bsp_Timer_Sub(Bsp_Timer3_GetMsCnt(), g_szDht11InitTime) * 1000 + Bsp_Timer3_GetUsCnt();
        g_szDht11Time[g_uiTimeIndex] &= 0x7fff;
        if (TRUE == Bsp_GetGpioStatus(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN))
            g_szDht11Time[g_uiTimeIndex] |= 0x8000;
        g_uiTimeIndex++;
        Gpio_ClearIrq(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN);    
    }
}

void Bsp_Dht11_Sync(void) {
    /* 初始化为输出 */
    Bsp_Gpio_OutPutInit(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, BSP_GPIO_PULL_DISABLE_E, 1);
    rt_thread_mdelay(100);

    /* 发送开始信号 */
    Bsp_SetGpioStatus(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, 0);
    rt_thread_mdelay(18);
    Bsp_SetGpioStatus(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, 1);

    /* 设置初始化时间 */
    g_szDht11InitTime = Bsp_Timer3_GetMsCnt();
    memset(g_szDht11Time, 0, sizeof(g_szDht11Time));
    g_uiTimeIndex = 0;

    /* 设置为输入并开启中断 */
    Bsp_Gpio_InPutInit(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, BSP_GPIO_PULL_DISABLE_E);
    // Gpio_EnableIrq(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, GpioIrqRising);
    Gpio_EnableIrq(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, GpioIrqFalling);
    EnableNvic(PORTB_IRQn, IrqLevel3, TRUE);

    int failedcnt = 3;
    while (failedcnt--) {
        rt_thread_mdelay(10);
        if (g_uiTimeIndex == BSP_DHT11_IRQTIME_MAXLEN - 1) {
            break;
        }
    }
    
    if (g_uiTimeIndex == BSP_DHT11_IRQTIME_MAXLEN - 1) {
        rt_kprintf("time : ");
        for (int i = 0; i < BSP_DHT11_IRQTIME_MAXLEN; i++) {
            rt_kprintf("%s %d ", (g_szDht11Time[i] & 0x8000) ? "+" : "-", g_szDht11Time[i] & 0x7fff);
        }
        rt_kprintf("\n");
    } else {
        Debug_Print("time recv failed index: %d\n", g_uiTimeIndex);
    }

    Bsp_Gpio_OutPutInit(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, BSP_GPIO_PULL_DISABLE_E, 1);
    Gpio_DisableIrq(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, GpioIrqRising);
    Gpio_DisableIrq(BSP_DHT11_DATA_GPIO, BSP_DHT11_DATA_PIN, GpioIrqFalling);
    return;
}
#endif
