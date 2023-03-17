#include "bsp_gpio.h"

void Bsp_Gpio_InPutInit(en_gpio_port_t enPort, en_gpio_pin_t enPin,  bsp_gpio_pullupdown_t enPull) {
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 打开GPIO外设时钟门控
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输入
    stcGpioCfg.enDir = GpioDirIn;
    ///< 端口驱动能力配置->高驱动能力
    stcGpioCfg.enDrv = GpioDrvH;
    ///< 端口上下拉配置
    if (enPull == BSP_GPIO_PULL_DISABLE_E) {
        stcGpioCfg.enPu = GpioPuDisable;
        stcGpioCfg.enPd = GpioPdDisable;
    } else if (enPull == BSP_GPIO_PULL_UP_E) {
        stcGpioCfg.enPu = GpioPuEnable;
        stcGpioCfg.enPd = GpioPdDisable;
    } else if (enPull == BSP_GPIO_PULL_DOWN_E) {
        stcGpioCfg.enPu = GpioPuDisable;
        stcGpioCfg.enPd = GpioPdEnable;
    }
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< 端口输入/输出值寄存器总线控制模式配置->AHB
    stcGpioCfg.enCtrlMode = GpioAHB;
    ///< GPIO IO USER KEY初始化
    Gpio_Init(enPort, enPin, &stcGpioCfg); 
    return;
}

void Bsp_Gpio_OutPutInit(en_gpio_port_t enPort, en_gpio_pin_t enPin,  bsp_gpio_pullupdown_t enPull,  uint8_t uiStatus) {
    stc_gpio_cfg_t stcGpioCfg;
    
    ///< 开启GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    ///< 端口方向配置->输出
    stcGpioCfg.enDir = GpioDirOut;
    ///< 端口驱动能力配置->高驱动能力
    stcGpioCfg.enDrv = GpioDrvH;

    ///< 端口上下拉配置
    if (enPull == BSP_GPIO_PULL_DISABLE_E) {
        stcGpioCfg.enPu = GpioPuDisable;
        stcGpioCfg.enPd = GpioPdDisable;
    } else if (enPull == BSP_GPIO_PULL_UP_E) {
        stcGpioCfg.enPu = GpioPuEnable;
        stcGpioCfg.enPd = GpioPdDisable;
    } else if (enPull == BSP_GPIO_PULL_DOWN_E) {
        stcGpioCfg.enPu = GpioPuDisable;
        stcGpioCfg.enPd = GpioPdEnable;
    }
    ///< 端口开漏输出配置->开漏输出关闭
    stcGpioCfg.enOD = GpioOdDisable;
    ///< GPIO IO (LED)初始化
    Gpio_Init(enPort, enPin, &stcGpioCfg);
    Bsp_SetGpioStatus(enPort, enPin, uiStatus);
    return;
}

void Bsp_SetGpioStatus(en_gpio_port_t enPort, en_gpio_pin_t enPin,  uint8_t uiStatus) {
    if (uiStatus) {
        Gpio_SetIO(enPort, enPin);    
    } else {
        Gpio_ClrIO(enPort, enPin);    
    }
    return;
}

boolean_t Bsp_GetGpioStatus(en_gpio_port_t enPort, en_gpio_pin_t enPin) {
    return Gpio_GetInputIO(enPort, enPin);
}