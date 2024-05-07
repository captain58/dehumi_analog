/******************************Copyright(c)******************************
**                          
**
** File Name: hal_iic_hrtc.c
** Author: 
** Date Last Update: 2019-11-02
** Description:  iic硬件光感传感器驱动
** Note: 
*******************************History***********************************
** Date: 2019-11-02
** Author: yzy
** Description: 文件创建
*************************************************************************/
#define EXT_IIC_LSENSOR

//	#include "sys.h"
#include "gpio.h"
#include "hal_iic_sht3.h"

#define CON_DELAY 200
/************************************************************************
*从器件地址
************************************************************************/
#define I2C_SHT_WR_ID  0x88//0x44//0x46                   //I2C从器件地址
#define I2C_SHT_RD_ID  0x89//0x45//47                   //I2C从器件地址




/************************************************************************
*硬件时钟IIC口端口全局结构体.
************************************************************************/
//	const IIC_PORTS* gsp_LSIICPrt;


static stc_gpio_cfg_t stcGpioOutCfg={
    .enDir = GpioDirOut,                           ///< 端口方向配置->输出    
    .enOD = GpioOdDisable,                          ///< 开漏输出
    .enPu = GpioPuEnable,                          ///< 端口上拉配置->使能
    .enPd = GpioPdDisable,                         ///< 端口下拉配置->禁止

};
static stc_gpio_cfg_t stcGpioInCfg={
    .enDir = GpioDirIn,                           ///< 端口方向配置->输出    
    .enOD = 1,                          ///< 开漏输出
    .enDrv=0,
    //.enPu = GpioPuEnable,                          ///< 端口上拉配置->使能
    //.enPd = GpioPdDisable,                         ///< 端口下拉配置->禁止
    .enPu = GpioPuDisable,
    .enPd = 1,
    //.enCtrlMode=0,

};
//	if (uiStatus) {
//	        Gpio_SetIO(enPort, enPin);    
//	    } else {
//	        Gpio_ClrIO(enPort, enPin);    
//	    }

/************************************************************************
*硬件时钟IIC口操作宏
************************************************************************/
#define I2C_SHT_GETSDA()    Gpio_GetInputIO(GpioPortB, GpioPin14)//(HAL_GPIO_GetPinState(gsp_LSIICPrt->portSDAIn->gpio, gsp_LSIICPrt->portSDAIn->pinnum))
//	HAL_GPIO_PinConfig(&gsp_LSIICPrt->portSDA->gpio[gsp_LSIICPrt->portSDA->pingrp], gsp_LSIICPrt->portSDA->pinnum, gsp_LSIICPrt->portSDA->type, gsp_LSIICPrt->portSDA->analog, 1);

#define I2C_SHT_SDAIN()    {Gpio_Init(GpioPortB,GpioPin14,&stcGpioInCfg);}
#define I2C_SHT_SDAOUT()   {Gpio_Init(GpioPortB,GpioPin14,&stcGpioOutCfg);}
#define I2C_SHT_SCKOUT()   {Gpio_Init(GpioPortB,GpioPin13,&stcGpioOutCfg);}

#define I2C_SHT_SDA_1()    {Gpio_SetIO(GpioPortB, GpioPin14);}
#define I2C_SHT_SDA_0()    {Gpio_ClrIO(GpioPortB, GpioPin14);}

#define I2C_SHT_SCK_1()    {Gpio_SetIO(GpioPortB, GpioPin13);}
#define I2C_SHT_SCK_0()    {Gpio_ClrIO(GpioPortB, GpioPin13);}

    
    
    

/*******************************************************************************
 * @function_name:  _DELAY
 * @function_file:  IIC_lsensor.C
 * @描述: 延时
 * 
 * 
 * @参数: 
 * @param: time  
 * @返回: 
 * @作者: yzy (2019-11-3)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
void _DELAY(uint16_t time)
{
    while(time--);
}

/*******************************************************************************
 * @function_name:  I2C_SHT_WriteByte
 * @function_file:  IIC_lsensor.C
 * @描述: I2C总线传输一字节数据
 * 
 * 
 * @参数: 
 * @param: byte  写入的字节数据
 * 
 * @返回: 
 * @return:  uint8_t   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8_t I2C_SHT_WriteByte(uint8_t byte)
{
    uint8_t err = 0;

    for(uint8_t uc_i = 0;uc_i < 8;uc_i++)     //循环处理各个数据位
    {
        I2C_SHT_SCK_0();                   //时钟线低
        _DELAY(CON_DELAY);                         //延时,保持
        if(byte & 0x80)                     //发送一位数据
        {
            I2C_SHT_SDA_1();
        }
        else
        {
            I2C_SHT_SDA_0();
        }
        _DELAY(CON_DELAY);                        //延时,保持
        I2C_SHT_SCK_1();                   //时钟搞电平
        _DELAY(CON_DELAY);                        //延时保持
        byte <<= 1;                         //移位
    }
                                            //等待ACK信号
    I2C_SHT_SCK_0();                       //时钟低
    _DELAY(CON_DELAY);  
    I2C_SHT_SDAIN();                       //数据线改为输入
    _DELAY(CON_DELAY);   
                                            //延时保持
    I2C_SHT_SCK_1();                       //时钟高
    _DELAY(CON_DELAY);                            //延时保持
    if(I2C_SHT_GETSDA())                   //获取数据线状态
    {
        err = 1;
        //return 1;                           //NAK,返回ERR
    }
    _DELAY(CON_DELAY);                             //延时保持
    I2C_SHT_SCK_0();                       //时钟低
    _DELAY(CON_DELAY);                             //延时保持
    //I2C_3231_SDA_1();
    I2C_SHT_SDAOUT();                      //数据线输出
    return err;
}

/*******************************************************************************
 * @function_name:  I2C_SHT_ReadByte
 * @function_file:  IIC_lsensor.C
 * @描述: I2C总线接收一字节数据(接收完一个字节后需要发送一个ACK或NAK信号)
 * 
 * @参数: 
 * 
 * @返回: IIC总线上收到的字节数据
 * @return:  uint8_t   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8_t I2C_SHT_ReadByte(void)
{
    uint8_t byte= 0;                          //接收的字节数据初始值

    I2C_SHT_SDAIN();                       //数据线输入
    _DELAY(CON_DELAY); 
    for(uint8_t uc_i = 0;uc_i < 8;uc_i++)     //循环接收各位数据
    {
        I2C_SHT_SCK_0();                   //时钟低
        _DELAY(CON_DELAY);                        //延时保持
        I2C_SHT_SCK_1();                   //时钟高
        _DELAY(CON_DELAY);                        //延时保持
        byte <<= 1;                         //数据位左移
        if(I2C_SHT_GETSDA())               //获取数据位
        {
            byte |= 0x01;
        }
        _DELAY(CON_DELAY);
    }

    return byte;
}

/*******************************************************************************
 * @function_name:  I2C_SHT_Start
 * @function_file:  IIC_lsensor.C
 * @描述: i2c总线启动
 * 
 * @参数: 
 * @返回: 
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
void I2C_SHT_Start(void)
{
    I2C_SHT_SDA_1();                       //数据线输出高
    I2C_SHT_SCK_1();                       //时钟线输出高
    _DELAY(CON_DELAY);                            //延时 保持
    I2C_SHT_SDA_0();                       //数据线下降沿
    _DELAY(CON_DELAY);                            //延时 保持
    I2C_SHT_SCK_0();                       //时钟线输出低
    _DELAY(CON_DELAY); 
    
}

/*******************************************************************************
 * @function_name:  I2C_SHT_Stop
 * @function_file:  IIC_lsensor.C
 * @描述: I2C总线停止
 * 
 * @参数: 
 * @返回: 
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
void I2C_SHT_Stop(void)
{
    //I2C_3231_SCK_0();                       //时钟线输出低
    I2C_SHT_SDA_0();                       //数据线输出低
    _DELAY(CON_DELAY);                            //延时 保持
    I2C_SHT_SCK_1();                       //时钟线输出高
    _DELAY(CON_DELAY);                            //延时 保持
    I2C_SHT_SDA_1();                       //数据线上升沿
    _DELAY(CON_DELAY);                            //延时 保持
    //I2C_3231_SCK_0();                       //时钟线输出低

}

/*******************************************************************************
 * @function_name:  I2C_SHT_ACK
 * @function_file:  IIC_lsensor.C
 * @描述: I2C总线ACK应答信号
 * 
 * @参数: 
 * @返回: 
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
void I2C_SHT_ACK(void)
{
    
    I2C_SHT_SCK_0();                       //时钟线低电平
    _DELAY(50);
    I2C_SHT_SDAOUT();
    _DELAY(100);
    I2C_SHT_SDA_0();                       //数据线低电平
    _DELAY(100);                            //延时 保持
    I2C_SHT_SCK_1();                       //时钟先上升沿
    _DELAY(200);                            //延时 保持
    I2C_SHT_SCK_0();                       //时钟线下降沿
}

/*******************************************************************************
 * @function_name:  I2C_SHT_NAK
 * @function_file:  IIC_lsensor.C
 * @描述: I2C总线NAK信号
 * 
 * @参数: 
 * @返回: 
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
void I2C_SHT_NAK(void)
{
    I2C_SHT_SCK_0();                       //时钟线低电平
    _DELAY(50);
    I2C_SHT_SDAOUT();
    _DELAY(100);
    I2C_SHT_SDA_1();                       //数据线搞电平
    _DELAY(100);                            //延时 保持
    I2C_SHT_SCK_1();                       //时钟上升沿
    _DELAY(200);                            //延时 保持
    I2C_SHT_SCK_0();                       //时钟线下降沿
}



/*******************************************************************************
 * @function_name:  I2C_SHT_ReadLX
 * @function_file:  IIC_lsensor.C
 * @描述: 光传感器数据读取
 * 
 * 
 * @参数: 
 * @param: date  数据缓存
 * 
 * @返回: 
 * @return:  uint8_t   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8_t I2C_SHT_Read(uint8_t* date)
{

    I2C_SHT_Start();                       //I2C总线启动               
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);         //写数据命令
    I2C_SHT_WriteByte(0x00);               //写入读取数据的地址
    
    I2C_SHT_Stop();
    I2C_SHT_Stop();

    I2C_SHT_Start();                       //I2C总线重新启动
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);     //读取数据命令
    for(uint8_t uc_i = 0; uc_i < 2; uc_i++)     //循环接收字节数据
    {
        date[uc_i] = I2C_SHT_ReadByte();   //读取一个字节数据
        if(uc_i == 1)                       //最后一个字节返回NAK
        {
            I2C_SHT_NAK();
        }
        else                                //返回ACK
        {
            I2C_SHT_ACK();
        }
    }
    I2C_SHT_Stop();                        //I2C总线停止
                                            //获取星期
//	    time[3] &= 0x7F;
//	    time[3] = STR_GetFirstSetBit(time + 3, 1);
    SYS_OK();
}
uint8_t I2C_SHT_Read_data(uint8_t* data, uint32_t u32Len)
//uint8_t *pu8Data,uint32_t u32Len
{
    I2C_SHT_Start();                       //I2C总线启动     
    //_DELAY(CON_DELAY); 
    //I2C_SHT_WriteByte(I2C_SHT_RD_ID);         //写数据命令
    //_DELAY(CON_DELAY); 
    //I2C_SHT_WriteByte(0x00);               //写入读取数据的地址
    //_DELAY(CON_DELAY); 
    //I2C_SHT_Stop();
    //_DELAY(CON_DELAY); 
    //I2C_SHT_Stop();
    //_DELAY(CON_DELAY); 
    //I2C_SHT_Start();                       //I2C总线重新启动
    //_DELAY(CON_DELAY); 
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);     //读取数据命令
    _DELAY(CON_DELAY); 
    for(uint8_t uc_i = 0; uc_i < u32Len; uc_i++)     //循环接收字节数据
    {
        data[uc_i] = I2C_SHT_ReadByte();   //读取一个字节数据
        if(uc_i == u32Len-1)                       //最后一个字节返回NAK
        {
            I2C_SHT_NAK();
        }
        else                                //返回ACK
        {
            I2C_SHT_ACK();
        }
    }
    I2C_SHT_Stop();                        //I2C总线停止

    SYS_OK();
    
}
/*******************************************************************************
 * @function_name:  I2C_SHT_WriteBcdTime
 * @function_file:  IIC_lsensor.C
 * @描述: 光传感器数据写入
 * 
 * 
 * @参数: 
 * @param: data  
 * 
 * @返回: 
 * @return:  uint8_t   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8_t I2C_SHT_Write(uint8_t data)
{
    uint8_t err = 1;                          //写入结果默认err

                                            //如果写入不成功则重写,最多重写3遍
    for(uint8_t uc_i = 0;(uc_i < 3) && (err != 0); uc_i++)
    {
        I2C_SHT_Start();                   //i2c总线启动
                                            //写命令
        err = I2C_SHT_WriteByte(I2C_SHT_WR_ID);
        //err |= I2C_SHT_WriteByte(0x00);    //写入操作地址

//	        for(uint8_t uc_j = 0;uc_j < 7;uc_j++) //循环写入各个字节
        {
          //_DELAY(50);
            err |= I2C_SHT_WriteByte(data);
        }
        I2C_SHT_Stop();                    //i2c总线停止
    }
    return err;                             //返回结果
}
uint8_t I2C_SHT_Write_Data(uint8_t *pu8Data,uint32_t u32Len)
{
    uint8_t err = 1;                          //写入结果默认err

                                            //如果写入不成功则重写,最多重写3遍
    for(uint8_t uc_i = 0;(uc_i < 3) && (err != 0); uc_i++)
    {
        I2C_SHT_Start();                   //i2c总线启动
                                            //写命令
        err = I2C_SHT_WriteByte(I2C_SHT_WR_ID);
        //err |= I2C_SHT_WriteByte(0x00);    //写入操作地址

        for(uint8_t uc_j = 0;uc_j < u32Len;uc_j++) //循环写入各个字节
        {
            _DELAY(50);
            err |= I2C_SHT_WriteByte(pu8Data[uc_j]);
        }
        I2C_SHT_Stop();                    //i2c总线停止
    }
    return err;                             //返回结果
    
    
//    en_result_t enRet = Error;
//    uint8_t u8i=0,u8State;
//    uint32_t uiFailed = 0;
//    I2C_SetFunc(I2CX,I2cStart_En);
//    while(1)
//    {
//        while(0 == I2C_GetIrq(I2CX))
//        {        
//            uiFailed++;
//            if (uiFailed > 0xffff) {
//                return Error;
//            }
//        }
//        u8State = I2C_GetState(I2CX);
//        switch(u8State)
//        {
//            case 0x08:                                 ///已发送起始条件
//                I2C_ClearFunc(I2CX,I2cStart_En);
//                I2C_WriteByte(I2CX,(I2C_DEVADDR<<1));  ///从设备地址发送
//                break;
//            case 0x18:                                 ///已发送SLA+W，并接收到ACK
//            case 0x28:                                 ///上一次发送数据后接收到ACK
//                I2C_WriteByte(I2CX,pu8Data[u8i++]);
//                break;
//            case 0x20:                                 ///上一次发送SLA+W后，收到NACK
//            case 0x38:                                 ///上一次在SLA+读或写时丢失仲裁
//                I2C_SetFunc(I2CX,I2cStart_En);         ///当I2C总线空闲时发送起始条件
//                break;
//            case 0x30:                                 ///已发送I2Cx_DATA中的数据，收到NACK，将传输一个STOP条件
//                I2C_SetFunc(I2CX,I2cStop_En);          ///发送停止条件
//                break;
//            default:
//                break;
//        }            
//        if(u8i>u32Len)
//        {
//            I2C_SetFunc(I2CX,I2cStop_En);              ///此顺序不能调换，出停止条件
//            I2C_ClearIrq(I2CX);
//            break;
//        }
//        I2C_ClearIrq(I2CX);                            ///清除中断状态标志位
//    }
//    enRet = Ok;
//    return enRet;
}

/*******************************************************************************
 * @function_name:  Init_I2C_LS
 * @function_file:  IIC_lsensor.C
 * @描述: 光传感器I2C总线初始化
 * 
 * @参数: 
 * @返回: 
 * @作者: yzy (2019-11-3)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
void Init_I2C_Sht(/*const IIC_PORTS* ports*/)
{
    uint8_t uc_i;
//	      __HAL_RCC_GPIOB_CLK_ENABLE();
                                            //配置SDA端口和SCK端口
//	    HAL_GPIO_PinConfig(ports->portSDAOut->gpio, ports->portSDAOut->pinnum, ports->portSDAOut->modefunc, ports->portSDAOut->dir);
//	    HAL_GPIO_PinConfig(ports->portSCK->gpio, ports->portSCK->pinnum, ports->portSCK->modefunc, ports->portSCK->dir);
    
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);   //开启GPIO时钟门控 
    
    stcGpioCfg.enDir = GpioDirOut;                           ///< 端口方向配置->输出    
    stcGpioCfg.enOD = GpioOdEnable;                          ///< 开漏输出
    stcGpioCfg.enPu = GpioPuEnable;                          ///< 端口上拉配置->使能
    stcGpioCfg.enPd = GpioPdDisable;                         ///< 端口下拉配置->禁止
        
    Gpio_Init(GpioPortB,GpioPin13,&stcGpioCfg);               ///< 端口初始化
    Gpio_Init(GpioPortB,GpioPin14,&stcGpioCfg);
    //    

    //赋值全局结构体
//	    gsp_LSIICPrt = ports;
//    I2C_SHT_SCK_0(); 
//    I2C_SHT_SDA_0(); 
    
    I2C_SHT_SDAIN();                       //数据信号线默认输入
    I2C_SHT_SCKOUT();                      //时钟信号线默认输出
    I2C_SHT_SCK_1();                       //默认输出低电平

    for(uc_i = 0;uc_i < 10;uc_i++)          //循环检测数据线状态,此时数据线为输入态
    {
        _DELAY(150);
        if(I2C_SHT_GETSDA())               //高电平,则产生一个Start信号
        {
            I2C_SHT_SDAOUT();
            I2C_SHT_SDA_1();
            break;
        }
                                            //数据线低电平,从器件输出,变换时钟线(上升沿),结束时序
        I2C_SHT_SCK_0();                   //时钟输出低
        _DELAY(200);                        //延时保持
        I2C_SHT_SCK_1();                   //时钟输出高(上升沿)
        _DELAY(200);                        //延时保持
    }
    if(uc_i == 10)                          //10个时钟上升沿,未能结束总线时序,出错
    {
        return;
    }
    I2C_SHT_SCK_0();
    _DELAY(200);
    I2C_SHT_SCK_1();
    _DELAY(150);
    I2C_SHT_SDA_1();

    //I2C_3231_F32KOFF();                     //默认关闭32K信号输出
   // I2C_SHT_SecIntOnOff(true);             //初始化打开秒中断输出
}


