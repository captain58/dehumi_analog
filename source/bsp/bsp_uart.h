#ifndef __BSP_UART_H__
#define __BSP_UART_H__

#include "ddl.h"
#include "uart.h"
#include "gpio.h"
#include "sysctrl.h"
#include <rtthread.h>
#include "bsp_ringbuf.h"

#define UART_RX_BUF_LEN 64
#define BSP_485EN_GPIO              (GpioPortA)
#define BSP_485EN_PIN          (GpioPin4)

int BspUartInit(void);
void Bsp_Uart_SendChar(int uiUart, char uiBuf);
int rt_rs485_getchar(void);
void Bsp_Rs485_SendData(char *pcData, uint32_t uilen);
/* 复位485 uart1 外设 */
void Bsp_Uart1_Reset(void);

#endif

