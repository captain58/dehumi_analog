#include "bsp_pm2_5.h"+
#include "bsp_uart.h"
#include "config.h"

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
    uint16_t result;
    /* 设置为输入并开启中断 */
    Bsp_Gpio_InPutInit(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, BSP_GPIO_PULL_DISABLE_E);
    Gpio_EnableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqRising);
    Gpio_EnableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqFalling);
    EnableNvic(PORTA_IRQn, IrqLevel3, TRUE);

    rt_thread_mdelay(BSP_PM2_5_CHECK_TIME * 1000);
    
    Gpio_DisableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqRising);
    Gpio_DisableIrq(BSP_PM2_5_DATA_GPIO, BSP_PM2_5_DATA_PIN, GpioIrqFalling);
    
#if (CON_PM25_PIRS10A > 0)
    result = g_pm2_5totalCnt / BSP_PM2_5_CHECK_TIME ;
    result /= 10;
#else
    
    result = g_pm2_5totalCnt * 1000 / Bsp_Timer_Sub(Bsp_Timer3_GetMsCnt(), g_pm2_5StartCnt);
    
    int fPm2 = result/10;
    result = (uint16_t)(((-0.0058 * fPm2 * fPm2 * fPm2)) + (0.42 * fPm2 * fPm2) + 11.5*fPm2 + 5);
#endif    
    return result;
}

uint16_t Bsp_Pm2_5_Init()
{
//    stc_gpio_cfg_t stcGpioCfg;

//    DDL_ZERO_STRUCT(stcGpioCfg);

//    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //使能GPIO模块时钟

//    ///<TX
//    stcGpioCfg.enDir = GpioDirOut;
//    Gpio_Init(GpioPortA, GpioPin9, &stcGpioCfg);
//    Gpio_SetAfMode(GpioPortA, GpioPin9, GpioAf2);          //配置PB06 端口为URART0_TX

//    ///<RX
//    stcGpioCfg.enDir = GpioDirIn;
//    Gpio_Init(GpioPortA, GpioPin10, &stcGpioCfg);
//    Gpio_SetAfMode(GpioPortA, GpioPin10, GpioAf2);          //配置PB07 端口为URART0_RX
//    
//    
////    stc_uart_cfg_t    stcCfg;

////    DDL_ZERO_STRUCT(stcCfg);

////    ///< 开启外设时钟
////    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);///<使能uart0模块时钟


////    ///<UART Init
////    stcCfg.enRunMode        = UartMskMode1;          ///<模式3
////    stcCfg.enStopBit        = UartMsk1bit;           ///<1bit停止位
////    stcCfg.stcBaud.u32Baud  = 9600;                  ///<波特率9600
////    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<通道采样分频配置
////    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<获得外设时钟（PCLK）频率值

////    Uart_Init(M0P_UART0, &stcCfg);                   ///<串口初始化


////    Uart_ClrStatus(M0P_UART0,UartRC);                ///<清接收请求
////    Uart_ClrStatus(M0P_UART0,UartTC);                ///<清接收请求
////    Uart_EnableIrq(M0P_UART0,UartRxIrq);             ///<使能串口接收中断
////    Uart_EnableIrq(M0P_UART0,UartFEIrq);             ///<使能串口错误中断
//    // Uart_EnableIrq(M0P_UART0,UartTxIrq);             ///<使能串口接收中断
//    
//        stc_uart_cfg_t  stcCfg;
//    stc_uart_multimode_t stcMulti;
//    stc_uart_baud_t stcBaud;

//    DDL_ZERO_STRUCT(stcCfg);
//    DDL_ZERO_STRUCT(stcMulti);
//    DDL_ZERO_STRUCT(stcBaud);
//    
//    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);//UART0外设模块时钟使能
//    
//    stcCfg.enRunMode = UartMskMode3;     //模式3
//    stcCfg.enStopBit = UartMsk1bit;      //1位停止位
//    stcCfg.enMmdorCk = UartMskDataOrAddr;      //偶校验
//    stcCfg.stcBaud.u32Baud = 9600;       //波特率9600
//    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;         //通道采样分频配置
//    stcCfg.stcBaud.u32Pclk = Sysctrl_GetPClkFreq();    //获得外设时钟（PCLK）频率值
//    Uart_Init(M0P_UART0, &stcCfg);       //串口初始化

//    Uart_ClrStatus(M0P_UART0,UartRC);    //清接收请求
//    Uart_ClrStatus(M0P_UART0,UartTC);    //清发送请求
//    Uart_EnableIrq(M0P_UART0,UartRxIrq); //使能串口接收中断
//    Uart_EnableIrq(M0P_UART0,UartTxIrq); //使能串口发送中断
//    
//    EnableNvic(UART0_IRQn, IrqLevel3, TRUE);       ///<系统中断使能    
//    
//    const uint8_t uccmd_to_pwm[]={0x42, 0x4d, 0xe4, 0x00, 0x00, 0x00, 0x00, 0x01, 0x73, 0x0d, 0x0a};
//    
//    Bsp_Uart_SendChars(0, (char *)uccmd_to_pwm, sizeof(uccmd_to_pwm));
//    
//    char retdata = rt_hw_console_getchar();
//    
//    return retdata;
    return 0;
}
