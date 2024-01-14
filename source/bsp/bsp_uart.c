/******************************************************************************
 * Include files
 ******************************************************************************/
#include "bsp_uart.h"
#include "bsp_gpio.h"
#include "reset.h"

rt_uint8_t uart_rx0_buf[UART_RX_BUF_LEN] = {0};
rt_uint8_t uart_rx1_buf[UART_RX_BUF_LEN] = {0};
static struct rt_ringbuffer  g_uart0_rxcb;         /* 定义一个 ringbuffer cb */
static struct rt_ringbuffer  g_uart1_rxcb;         /* 定义一个 ringbuffer cb */
static struct rt_semaphore g_shell_rx_sem; /* 定义一个静态信号量 */
static struct rt_semaphore g_uart_rx_sem; /* 定义一个静态信号量 */

/**
 ******************************************************************************
 ** \brief  Main function of project
 **
 ** \return uint32_t return value, if needed
 **
 ** This sample
 **
 ******************************************************************************/

char rt_hw_console_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

    /* 从 ringbuffer 中拿出数据 */
    while (rt_ringbuffer_getchar(&g_uart0_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&g_shell_rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

int rt_rs485_getchar(void)
{
    /* Note: the initial value of ch must < 0 */
    int ch = -1;

    /* 从 ringbuffer 中拿出数据 */
    while (rt_ringbuffer_getchar(&g_uart1_rxcb, (rt_uint8_t *)&ch) != 1)
    {
        rt_sem_take(&g_uart_rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

//UART1中断函数
void Uart1_IRQHandler(void)
{
    int ch = -1;
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    if(Uart_GetStatus(M0P_UART1, UartRC))         //UART1数据接收
    {
        Uart_ClrStatus(M0P_UART1, UartRC);        //清中断状态位
        ch = Uart_ReceiveData(M0P_UART1);   //接收数据字节
        if (ch != -1) {
            /* 读取到数据，将数据存入 ringbuffer */
            if (0 == rt_ringbuffer_putchar(&g_uart1_rxcb, ch)) {
                rt_uint8_t chtemp;
                rt_ringbuffer_getchar(&g_uart1_rxcb, &chtemp);
                rt_ringbuffer_putchar(&g_uart1_rxcb, ch);
            }
            rt_sem_release(&g_uart_rx_sem);
        }
    }
    if(Uart_GetStatus(M0P_UART1, UartFE))   
    {
        Uart_ClrStatus(M0P_UART1, UartFE);   
    }
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}

//UART1中断函数
void Uart0_IRQHandler(void)
{
    int ch = -1;
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    if(Uart_GetStatus(M0P_UART0, UartRC))         //UART1数据接收
    {
        Uart_ClrStatus(M0P_UART0, UartRC);        //清中断状态位
        ch = Uart_ReceiveData(M0P_UART0);   //接收数据字节
        if (ch != -1) {
            /* 读取到数据，将数据存入 ringbuffer */
            if (0 == rt_ringbuffer_putchar(&g_uart0_rxcb, ch)) {
                rt_uint8_t chtemp;
                rt_ringbuffer_getchar(&g_uart0_rxcb, &chtemp);
                rt_ringbuffer_putchar(&g_uart0_rxcb, ch);
            }
            rt_sem_release(&g_shell_rx_sem);
        }
    }
    if(Uart_GetStatus(M0P_UART0, UartFE))   
    {
        Uart_ClrStatus(M0P_UART0, UartFE);   
    }
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}

//串口引脚配置
void App_PortInit(void)
{
    stc_gpio_cfg_t stcGpioCfg;

    DDL_ZERO_STRUCT(stcGpioCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE); //使能GPIO模块时钟

    ///<TX
    stcGpioCfg.enDir = GpioDirOut;
    Gpio_Init(GpioPortA, GpioPin2, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortA, GpioPin2, GpioAf1);          //配置PA02 端口为URART1_TX
    Gpio_Init(GpioPortB, GpioPin6, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortB, GpioPin6, GpioAf2);          //配置PB06 端口为URART0_TX

    ///<RX
    stcGpioCfg.enDir = GpioDirIn;
    Gpio_Init(GpioPortA, GpioPin3, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortA, GpioPin3, GpioAf1);          //配置PA03 端口为URART1_RX
    Gpio_Init(GpioPortB, GpioPin7, &stcGpioCfg);
    Gpio_SetAfMode(GpioPortB, GpioPin7, GpioAf2);          //配置PB07 端口为URART0_RX
}

//串口配置
void App_UartCfg(void)
{
    stc_uart_cfg_t    stcCfg;

    DDL_ZERO_STRUCT(stcCfg);

    ///< 开启外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart0,TRUE);///<使能uart0模块时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralUart1,TRUE);///<使能uart1模块时钟

    ///<UART Init
    stcCfg.enRunMode        = UartMskMode1;          ///<模式3
    stcCfg.enStopBit        = UartMsk1bit;           ///<1bit停止位
    stcCfg.stcBaud.u32Baud  = 9600;                  ///<波特率9600
    stcCfg.stcBaud.enClkDiv = UartMsk8Or16Div;       ///<通道采样分频配置
    stcCfg.stcBaud.u32Pclk  = Sysctrl_GetPClkFreq(); ///<获得外设时钟（PCLK）频率值
    Uart_Init(M0P_UART1, &stcCfg);                   ///<串口初始化
    stcCfg.stcBaud.u32Baud  = 115200;                  ///<波特率9600
    Uart_Init(M0P_UART0, &stcCfg);                   ///<串口初始化

    ///<UART中断使能
    Uart_ClrStatus(M0P_UART1,UartRC);                ///<清接收请求
    Uart_ClrStatus(M0P_UART1,UartTC);                ///<清接收请求
    Uart_EnableIrq(M0P_UART1,UartRxIrq);             ///<使能串口接收中断
    Uart_EnableIrq(M0P_UART1,UartFEIrq);             ///<使能串口错误中断
    // Uart_EnableIrq(M0P_UART1,UartTxIrq);             ///<使能串口接收中断
    EnableNvic(UART1_IRQn, IrqLevel3, TRUE);       ///<系统中断使能

    Uart_ClrStatus(M0P_UART0,UartRC);                ///<清接收请求
    Uart_ClrStatus(M0P_UART0,UartTC);                ///<清接收请求
    Uart_EnableIrq(M0P_UART0,UartRxIrq);             ///<使能串口接收中断
    Uart_EnableIrq(M0P_UART0,UartFEIrq);             ///<使能串口错误中断
    // Uart_EnableIrq(M0P_UART0,UartTxIrq);             ///<使能串口接收中断
    EnableNvic(UART0_IRQn, IrqLevel3, TRUE);       ///<系统中断使能
    return;
}

void Bsp_Uart_Init(void) {
    //串口引脚配置
    App_PortInit();

    // 485控制引脚配置
    Bsp_Gpio_OutPutInit(BSP_485EN_GPIO, BSP_485EN_PIN, BSP_GPIO_PULL_DOWN_E, 0);
	
    //串口配置
    App_UartCfg();
    return;
}

int BspUartInit(void)
{
    /* 初始化串口接收 ringbuffer  */
    rt_ringbuffer_init(&g_uart0_rxcb, uart_rx0_buf, UART_RX_BUF_LEN);
    rt_ringbuffer_init(&g_uart1_rxcb, uart_rx1_buf, UART_RX_BUF_LEN);

    /* 初始化串口接收数据的信号量 */
    rt_sem_init(&(g_shell_rx_sem), "shell_rx", 0, 0);
    rt_sem_init(&(g_uart_rx_sem), "uart_rx", 0, 0);

    Bsp_Uart_Init();

    return 0;
}

/* 复位485 uart1 外设 */
void Bsp_Uart1_Reset(void) {
    Reset_RstPeripheral(ResetMskUart1);
    Reset_RstPeripheral(ResetMskUart0);
    Bsp_Uart_Init();
    return;
}

void Bsp_Uart_SendChar(int uiUart, char uiBuf) {
    if (uiUart == 0) {
        Uart_SendDataPoll(M0P_UART0, uiBuf);
    } else if (uiUart == 1) {
        Uart_SendDataPoll(M0P_UART1, uiBuf);
    }

    return;
}

void Bsp_Rs485_SendData(char *pcData, uint32_t uilen) {
    Bsp_SetGpioStatus(BSP_485EN_GPIO, BSP_485EN_PIN, 1);
    delay10us(2);
    for (int i = 0; i < uilen; i++) {
        Bsp_Uart_SendChar(1, pcData[i]);
    }
    
    delay10us(2);
    Bsp_SetGpioStatus(BSP_485EN_GPIO, BSP_485EN_PIN, 0);
    return;
}

/******************************************************************************
 * EOF (not truncated)
 ******************************************************************************/

