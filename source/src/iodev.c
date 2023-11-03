#include "src/iodev.h"
#include "config.h"
boolean_t IoDevGetStatus(en_gpio_port_t enPort, en_gpio_pin_t enPin) {
    return Bsp_GetGpioStatus(enPort, enPin);
}

boolean_t IoDevGetElectricMachineryStatus(void) {
    if (Bsp_EleCheck_Sync()) {
        return TRUE;
    } else {
        return FALSE;
    }
}

/* 设置压缩机状态 */
void IoDevSetCompressor(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(COMPERSSOR_GPIO, COMPERSSOR_PIN, 1);
    } else {
        Bsp_SetGpioStatus(COMPERSSOR_GPIO, COMPERSSOR_PIN, 0);
    }
}
/* 设置加湿水泵状态 */
void IoDevSetWaterPump(uint8_t flag) {
    Bsp_Timer_PwmEnable(BSP_PWM_WATERPUMP, flag);
    return;
}
/* 设置紫外灯状态 */
void IoDevSetUVLamp(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(UVLAMP_GPIO, UVLAMP_PIN, 1);
    } else {
        Bsp_SetGpioStatus(UVLAMP_GPIO, UVLAMP_PIN, 0);
    }
}
/* 设置负离子状态 */
void IoDevSetAnion(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(ANION_GPIO, ANION_PIN, 1);
    } else {
        Bsp_SetGpioStatus(ANION_GPIO, ANION_PIN, 0);
    }
}
/* 设置电机状态 ELEMAC_LEVEL_E */
void IoDevSetElectricMachinery(uint8_t flag) {
    Bsp_Timer_PwmEnable(BSP_PWM_FENGSHAN, flag);
}
/* 设置臭氧状态 */
void IoDevSetOzone(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(OZONE_GPIO, OZONE_PIN, 1);
    } else {
        Bsp_SetGpioStatus(OZONE_GPIO, OZONE_PIN, 0);
    }
}
/* 设置百叶风机状态 */
void IoDevSetBaiYe(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(BAIYEFENGJI_GPIO, BAIYEFENGJI_PIN, 1);
    } else {
        Bsp_SetGpioStatus(BAIYEFENGJI_GPIO, BAIYEFENGJI_PIN, 0);
    }
}

/* 设置排水状态 */
void IoDevSetPaishui(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(PAISHUI_GPIO, PAISHUI_PIN, 1);
    } else {
        Bsp_SetGpioStatus(PAISHUI_GPIO, PAISHUI_PIN, 0);
    }
}

/* 设置LED状态 */
void IoDevSetLED(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(LED_GPIO, LED_PIN, 1);
    } else {
        Bsp_SetGpioStatus(LED_GPIO, LED_PIN, 0);
    }
}

/* 设置IoDevSetRH_IO1状态 */
void IoDevSetRH_IO1(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(RH_IO1_GPIO, RH_IO1_PIN, 1);
    } else {
        Bsp_SetGpioStatus(RH_IO1_GPIO, RH_IO1_PIN, 0);
    }
}

/* 设置IoDevSetRH_IO2状态 */
void IoDevSetRH_IO2(uint8_t flag) {
    if (flag) {
        Bsp_SetGpioStatus(RH_IO2_GPIO, RH_IO2_PIN, 1);
    } else {
        Bsp_SetGpioStatus(RH_IO2_GPIO, RH_IO2_PIN, 0);
    }
}

static int IoDevInit(void) {
    Bsp_Gpio_InPutInit(WATER_LEAKAGE_GPIO, WATER_LEAKAGE_PIN, BSP_GPIO_PULL_DISABLE_E);
    Bsp_Gpio_InPutInit(LOW_WATER_LEVEL_GPIO, LOW_WATER_LEVEL_PIN, BSP_GPIO_PULL_DISABLE_E);
    Bsp_Gpio_InPutInit(UPPER_WATER_LEVEL_GPIO, UPPER_WATER_LEVEL_PIN, BSP_GPIO_PULL_DISABLE_E);
    Bsp_Gpio_InPutInit(MID_WATER_LEVEL_GPIO, MID_WATER_LEVEL_PIN, BSP_GPIO_PULL_DISABLE_E);
    Bsp_Gpio_InPutInit(COMPERSSORPRESSURE_GPIO, COMPERSSORPRESSURE_PIN, BSP_GPIO_PULL_DISABLE_E);

    Bsp_Gpio_OutPutInit(COMPERSSOR_GPIO, COMPERSSOR_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(WATER_PUMP_GPIO, WATER_PUMP_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(UVLAMP_GPIO, UVLAMP_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(ANION_GPIO, ANION_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(OZONE_GPIO, OZONE_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(BAIYEFENGJI_GPIO, BAIYEFENGJI_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(PAISHUI_GPIO, PAISHUI_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(LED_GPIO, LED_PIN, BSP_GPIO_PULL_UP_E, 1);
    Bsp_Gpio_OutPutInit(RH_IO1_GPIO, RH_IO1_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    Bsp_Gpio_OutPutInit(RH_IO2_GPIO, RH_IO2_PIN, BSP_GPIO_PULL_DOWN_E, 0);
    return 0;
}

INIT_BOARD_EXPORT(IoDevInit);
