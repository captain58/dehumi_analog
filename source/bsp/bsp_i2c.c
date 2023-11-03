#include "bsp_i2c.h"

///< IO端口配置
void App_PortCfg(void)
{
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);   //开启GPIO时钟门控 
    
    stcGpioCfg.enDir = GpioDirOut;                           ///< 端口方向配置->输出    
    stcGpioCfg.enOD = GpioOdEnable;                          ///< 开漏输出
    stcGpioCfg.enPu = GpioPuEnable;                          ///< 端口上拉配置->使能
    stcGpioCfg.enPd = GpioPdDisable;                         ///< 端口下拉配置->禁止
    
//    Gpio_Init(GpioPortB,GpioPin13,&stcGpioCfg);               ///< 端口初始化
//    Gpio_Init(GpioPortB,GpioPin14,&stcGpioCfg);
//    
//    Gpio_SetAfMode(GpioPortB,GpioPin13,GpioAf2);              ///< 配置PB08为SCL
//    Gpio_SetAfMode(GpioPortB,GpioPin14,GpioAf2);              ///< 配置PB09为SDA
}

///< I2C 模块配置
void App_I2cCfg(void)
{
    stc_i2c_cfg_t stcI2cCfg;
    
    DDL_ZERO_STRUCT(stcI2cCfg);                            ///< 初始化结构体变量的值为0
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralI2c1,TRUE); ///< 开启I2C0时钟门控
    
    stcI2cCfg.u32Pclk = Sysctrl_GetPClkFreq();             ///< 获取PCLK时钟
    stcI2cCfg.u32Baud = 400000;                           ///< 频率
    stcI2cCfg.enMode = I2cMasterMode;                      ///< 主机模式
    stcI2cCfg.u8SlaveAddr = I2C_DEVADDR;                   ///< 从地址，主模式无效
    stcI2cCfg.bGc = FALSE;                                 ///< 广播地址应答使能关闭
    I2C_Init(M0P_I2C1,&stcI2cCfg);                         ///< 模块初始化
}

/**
 ******************************************************************************
 ** \brief  主机接收函数
 **
 ** \param u8Addr从机内存地址，pu8Data读数据存放缓存，u32Len读数据长度
 **
 ** \retval 读数据是否成功
 **
 ******************************************************************************/
en_result_t I2C_MasterReadData(M0P_I2C_TypeDef* I2CX,uint8_t *pu8Data,uint32_t u32Len, uint32_t uiTimeOut)
{
    en_result_t enRet = Error;
    uint8_t u8i=0,u8State;
    uint32_t uiTime0 = rt_tick_get();    
    uint32_t uiFailed = 0;

    I2C_SetFunc(I2CX,I2cStart_En);
    
    while(1)
    {
        while(0 == I2C_GetIrq(I2CX))
        {        
            uiFailed++;
            if (uiFailed > 0xffff) {
                return Error;
            }
        }
        u8State = I2C_GetState(I2CX);
        switch(u8State)
        {
            case 0x08:                                    //已发送起始条件，将发送SLA+R
                I2C_ClearFunc(I2CX,I2cStart_En);
                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1)|0x01);//发送SLA+W
                break;
            case 0x18:                                    //已发送SLA+W,并接收到ACK
                I2C_WriteByte(I2CX,0);                    //发送内存地址
                break;
            case 0x28:                                    //已发送数据，接收到ACK
                I2C_SetFunc(I2CX,I2cStart_En);
                break;
            case 0x10:                                    //已发送重复起始条件
                I2C_ClearFunc(I2CX,I2cStart_En);
                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1)|0x01);//读命令发送
                break;
            case 0x40:                                    //已发送SLA+R，并接收到ACK
                if(u32Len>1)
                {
                    I2C_SetFunc(I2CX,I2cAck_En);
                }
                break;
            case 0x50:                                    //已接收数据字节，并已返回ACK信号
                pu8Data[u8i++] = I2C_ReadByte(I2CX);
                if(u8i==u32Len-1)
                {
                    I2C_ClearFunc(I2CX,I2cAck_En);        //读数据时，倒数第二个字节ACK关闭
                }
                break;
            case 0x58:                                    //已接收到最后一个数据，NACK已返回
                pu8Data[u8i++] = I2C_ReadByte(I2CX);
                I2C_SetFunc(I2CX,I2cStop_En);             //发送停止条件
                break;    
            case 0x38:                                    //在发送地址或数据时，仲裁丢失
                I2C_SetFunc(I2CX,I2cStart_En);            //当总线空闲时发起起始条件
                break;
            case 0x48:                                    //发送SLA+R后，收到一个NACK
                I2C_SetFunc(I2CX,I2cStop_En);
                I2C_SetFunc(I2CX,I2cStart_En);
                break;
            default:                                      //其他错误状态，重新发送起始条件
                I2C_SetFunc(I2CX,I2cStart_En);            //其他错误状态，重新发送起始条件
                break;
        }
        I2C_ClearIrq(I2CX);                               //清除中断状态标志位
        if(u8i==u32Len)                                   //数据全部读取完成，跳出while循环
        {
            break;
        }
        if (rt_tick_get() - uiTime0 > uiTimeOut) {
            return ErrorTimeout;
        }
        rt_thread_mdelay(10);
    }
    enRet = Ok;
    return enRet;
}
/**
 ******************************************************************************
 ** \brief  主机发送函数
 **
 ** \param u8Addr从机内存地址，pu8Data写数据，u32Len写数据长度
 **
 ** \retval 写数据是否成功
 **
 ******************************************************************************/
en_result_t I2C_MasterWriteData(M0P_I2C_TypeDef* I2CX,uint8_t *pu8Data,uint32_t u32Len)
{
    en_result_t enRet = Error;
    uint8_t u8i=0,u8State;
    uint32_t uiFailed = 0;
    I2C_SetFunc(I2CX,I2cStart_En);
    while(1)
    {
        while(0 == I2C_GetIrq(I2CX))
        {        
            uiFailed++;
            if (uiFailed > 0xffff) {
                return Error;
            }
        }
        u8State = I2C_GetState(I2CX);
        switch(u8State)
        {
            case 0x08:                                 ///已发送起始条件
                I2C_ClearFunc(I2CX,I2cStart_En);
                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1));  ///从设备地址发送
                break;
            case 0x18:                                 ///已发送SLA+W，并接收到ACK
            case 0x28:                                 ///上一次发送数据后接收到ACK
                I2C_WriteByte(I2CX,pu8Data[u8i++]);
                break;
            case 0x20:                                 ///上一次发送SLA+W后，收到NACK
            case 0x38:                                 ///上一次在SLA+读或写时丢失仲裁
                I2C_SetFunc(I2CX,I2cStart_En);         ///当I2C总线空闲时发送起始条件
                break;
            case 0x30:                                 ///已发送I2Cx_DATA中的数据，收到NACK，将传输一个STOP条件
                I2C_SetFunc(I2CX,I2cStop_En);          ///发送停止条件
                break;
            default:
                break;
        }            
        if(u8i>u32Len)
        {
            I2C_SetFunc(I2CX,I2cStop_En);              ///此顺序不能调换，出停止条件
            I2C_ClearIrq(I2CX);
            break;
        }
        I2C_ClearIrq(I2CX);                            ///清除中断状态标志位
    }
    enRet = Ok;
    return enRet;
}

boolean_t Bsp_I2c_WriteData(uint8_t *pu8Data,uint32_t u32Len) {
    if (I2C_MasterWriteData(M0P_I2C1, pu8Data, u32Len) == Ok) {
        return TRUE;
    } else {
        return FALSE;
    }
}

boolean_t Bsp_I2c_ReadData(uint8_t *pu8Data,uint32_t u32Len, uint32_t uiTimeOut) {
    if (I2C_MasterReadData(M0P_I2C1, pu8Data, u32Len, uiTimeOut) == Ok) {
        return TRUE;
    } else {
        return FALSE;
    }
}

static int Bsp_I2c_Init(void) {
    ///< I2C 模块配置
    App_I2cCfg();

    ///< IO端口配置
    App_PortCfg();
    
    return 0;
}

INIT_BOARD_EXPORT(Bsp_I2c_Init);
