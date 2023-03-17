#ifndef __SRC_RS485PRO_H__
#define __SRC_RS485PRO_H__
#include <rtthread.h>

#include "bsp/bsp_uart.h"

union Fdata 
{
  float fvalue; 
  uint8_t szdata[4];
};

void Debug_DevCurMode_(uint8_t enRunMode);

#endif
