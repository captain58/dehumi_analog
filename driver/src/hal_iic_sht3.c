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
#include "hal_iic_sht3.h"


/************************************************************************
*从器件地址
************************************************************************/
//#define I2C_SHT_WR_ID  0x46                   //I2C从器件地址
#define I2C_SHT_RD_ID  0x47                   //I2C从器件地址




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
    //.enOD = GpioOdDisable,                          ///< 开漏输出
    //.enPu = GpioPuEnable,                          ///< 端口上拉配置->使能
    //.enPd = GpioPdDisable,                         ///< 端口下拉配置->禁止
    .enPu = GpioPuDisable;
    .enPd = GpioPdEnable;

};
//	if (uiStatus) {
//	        Gpio_SetIO(enPort, enPin);    
//	    } else {
//	        Gpio_ClrIO(enPort, enPin);    
//	    }

/************************************************************************
*硬件时钟IIC口操作宏
************************************************************************/
#define I2C_SHT_GETSDA()    (HAL_GPIO_GetPinState(gsp_LSIICPrt->portSDAIn->gpio, gsp_LSIICPrt->portSDAIn->pinnum))
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
void _DELAY(uint16 time)
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
 * @return:  uint8   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8 I2C_SHT_WriteByte(uint8 byte)
{
    uint8 err = 0;

    for(uint8 uc_i = 0;uc_i < 8;uc_i++)     //循环处理各个数据位
    {
        I2C_SHT_SCK_0();                   //时钟线低
        _DELAY(100);                         //延时,保持
        if(byte & 0x80)                     //发送一位数据
        {
            I2C_SHT_SDA_1();
        }
        else
        {
            I2C_SHT_SDA_0();
        }
        _DELAY(150);                        //延时,保持
        I2C_SHT_SCK_1();                   //时钟搞电平
        _DELAY(200);                        //延时保持
        byte <<= 1;                         //移位
    }
                                            //等待ACK信号
    I2C_SHT_SCK_0();                       //时钟低
    _DELAY(50);  
    I2C_SHT_SDAIN();                       //数据线改为输入
    _DELAY(150);   
                                            //延时保持
    I2C_SHT_SCK_1();                       //时钟高
    _DELAY(100);                            //延时保持
    if(I2C_SHT_GETSDA())                   //获取数据线状态
    {
        err = 1;
        //return 1;                           //NAK,返回ERR
    }
    _DELAY(50);                             //延时保持
    I2C_SHT_SCK_0();                       //时钟低
    _DELAY(50);                             //延时保持
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
 * @return:  uint8   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8 I2C_SHT_ReadByte(void)
{
    uint8 byte= 0;                          //接收的字节数据初始值

    I2C_SHT_SDAIN();                       //数据线输入
    for(uint8 uc_i = 0;uc_i < 8;uc_i++)     //循环接收各位数据
    {
        I2C_SHT_SCK_0();                   //时钟低
        _DELAY(200);                        //延时保持
        I2C_SHT_SCK_1();                   //时钟高
        _DELAY(150);                        //延时保持
        byte <<= 1;                         //数据位左移
        if(I2C_SHT_GETSDA())               //获取数据位
        {
            byte |= 0x01;
        }
        _DELAY(50);
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
    _DELAY(200);                            //延时 保持
    I2C_SHT_SDA_0();                       //数据线下降沿
    _DELAY(200);                            //延时 保持
    I2C_SHT_SCK_0();                       //时钟线输出低
    
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
    _DELAY(200);                            //延时 保持
    I2C_SHT_SCK_1();                       //时钟线输出高
    _DELAY(200);                            //延时 保持
    I2C_SHT_SDA_1();                       //数据线上升沿
    //_DELAY(100);                            //延时 保持
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
 * @return:  uint8   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8 I2C_SHT_Read(uint8* date)
{

    I2C_SHT_Start();                       //I2C总线启动               
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);         //写数据命令
    I2C_SHT_WriteByte(0x00);               //写入读取数据的地址
    
    I2C_SHT_Stop();
    I2C_SHT_Stop();

    I2C_SHT_Start();                       //I2C总线重新启动
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);     //读取数据命令
    for(uint8 uc_i = 0; uc_i < 2; uc_i++)     //循环接收字节数据
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
 * @return:  uint8   
 * @作者: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @修改人: 
 * @修改说明: 
 ******************************************************************************/
uint8 I2C_SHT_Write(uint8 data)
{
    uint8 err = 1;                          //写入结果默认err

                                            //如果写入不成功则重写,最多重写3遍
    for(uint8 uc_i = 0;(uc_i < 3) && (err != 0); uc_i++)
    {
        I2C_SHT_Start();                   //i2c总线启动
                                            //写命令
        err = I2C_SHT_WriteByte(I2C_SHT_WR_ID);
        //err |= I2C_SHT_WriteByte(0x00);    //写入操作地址

//	        for(uint8 uc_j = 0;uc_j < 7;uc_j++) //循环写入各个字节
        {
          //_DELAY(50);
            err |= I2C_SHT_WriteByte(data);
        }
        I2C_SHT_Stop();                    //i2c总线停止
    }
    return err;                             //返回结果
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
    uint8 uc_i;
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


