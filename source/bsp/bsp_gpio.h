#ifndef __BSP_GPIO_H__
#define __BSP_GPIO_H__

#include "sysctrl.h"
#include "gpio.h"

typedef enum bsp_gpio_pullupdown
{
    BSP_GPIO_PULL_DISABLE_E,
    BSP_GPIO_PULL_UP_E,
    BSP_GPIO_PULL_DOWN_E,
}bsp_gpio_pullupdown_t;

void Bsp_Gpio_OutPutInit(en_gpio_port_t enPort, en_gpio_pin_t enPin,  bsp_gpio_pullupdown_t enPull,  uint8_t uiStatus);
void Bsp_Gpio_InPutInit(en_gpio_port_t enPort, en_gpio_pin_t enPin,  bsp_gpio_pullupdown_t enPull);

void Bsp_SetGpioStatus(en_gpio_port_t enPort, en_gpio_pin_t enPin,  uint8_t uiStatus);
boolean_t Bsp_GetGpioStatus(en_gpio_port_t enPort, en_gpio_pin_t enPin);

#endif
