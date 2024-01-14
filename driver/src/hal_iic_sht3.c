/******************************Copyright(c)******************************
**                          
**
** File Name: hal_iic_hrtc.c
** Author: 
** Date Last Update: 2019-11-02
** Description:  iicӲ����д���������
** Note: 
*******************************History***********************************
** Date: 2019-11-02
** Author: yzy
** Description: �ļ�����
*************************************************************************/
#define EXT_IIC_LSENSOR

//	#include "sys.h"
#include "hal_iic_sht3.h"


/************************************************************************
*��������ַ
************************************************************************/
//#define I2C_SHT_WR_ID  0x46                   //I2C��������ַ
#define I2C_SHT_RD_ID  0x47                   //I2C��������ַ




/************************************************************************
*Ӳ��ʱ��IIC�ڶ˿�ȫ�ֽṹ��.
************************************************************************/
//	const IIC_PORTS* gsp_LSIICPrt;


static stc_gpio_cfg_t stcGpioOutCfg={
    .enDir = GpioDirOut,                           ///< �˿ڷ�������->���    
    .enOD = GpioOdDisable,                          ///< ��©���
    .enPu = GpioPuEnable,                          ///< �˿���������->ʹ��
    .enPd = GpioPdDisable,                         ///< �˿���������->��ֹ

};
static stc_gpio_cfg_t stcGpioInCfg={
    .enDir = GpioDirIn,                           ///< �˿ڷ�������->���    
    //.enOD = GpioOdDisable,                          ///< ��©���
    //.enPu = GpioPuEnable,                          ///< �˿���������->ʹ��
    //.enPd = GpioPdDisable,                         ///< �˿���������->��ֹ
    .enPu = GpioPuDisable;
    .enPd = GpioPdEnable;

};
//	if (uiStatus) {
//	        Gpio_SetIO(enPort, enPin);    
//	    } else {
//	        Gpio_ClrIO(enPort, enPin);    
//	    }

/************************************************************************
*Ӳ��ʱ��IIC�ڲ�����
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
 * @����: ��ʱ
 * 
 * 
 * @����: 
 * @param: time  
 * @����: 
 * @����: yzy (2019-11-3)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
void _DELAY(uint16 time)
{
    while(time--);
}

/*******************************************************************************
 * @function_name:  I2C_SHT_WriteByte
 * @function_file:  IIC_lsensor.C
 * @����: I2C���ߴ���һ�ֽ�����
 * 
 * 
 * @����: 
 * @param: byte  д����ֽ�����
 * 
 * @����: 
 * @return:  uint8   
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
uint8 I2C_SHT_WriteByte(uint8 byte)
{
    uint8 err = 0;

    for(uint8 uc_i = 0;uc_i < 8;uc_i++)     //ѭ�������������λ
    {
        I2C_SHT_SCK_0();                   //ʱ���ߵ�
        _DELAY(100);                         //��ʱ,����
        if(byte & 0x80)                     //����һλ����
        {
            I2C_SHT_SDA_1();
        }
        else
        {
            I2C_SHT_SDA_0();
        }
        _DELAY(150);                        //��ʱ,����
        I2C_SHT_SCK_1();                   //ʱ�Ӹ��ƽ
        _DELAY(200);                        //��ʱ����
        byte <<= 1;                         //��λ
    }
                                            //�ȴ�ACK�ź�
    I2C_SHT_SCK_0();                       //ʱ�ӵ�
    _DELAY(50);  
    I2C_SHT_SDAIN();                       //�����߸�Ϊ����
    _DELAY(150);   
                                            //��ʱ����
    I2C_SHT_SCK_1();                       //ʱ�Ӹ�
    _DELAY(100);                            //��ʱ����
    if(I2C_SHT_GETSDA())                   //��ȡ������״̬
    {
        err = 1;
        //return 1;                           //NAK,����ERR
    }
    _DELAY(50);                             //��ʱ����
    I2C_SHT_SCK_0();                       //ʱ�ӵ�
    _DELAY(50);                             //��ʱ����
    //I2C_3231_SDA_1();
    I2C_SHT_SDAOUT();                      //���������
    return err;
}

/*******************************************************************************
 * @function_name:  I2C_SHT_ReadByte
 * @function_file:  IIC_lsensor.C
 * @����: I2C���߽���һ�ֽ�����(������һ���ֽں���Ҫ����һ��ACK��NAK�ź�)
 * 
 * @����: 
 * 
 * @����: IIC�������յ����ֽ�����
 * @return:  uint8   
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
uint8 I2C_SHT_ReadByte(void)
{
    uint8 byte= 0;                          //���յ��ֽ����ݳ�ʼֵ

    I2C_SHT_SDAIN();                       //����������
    for(uint8 uc_i = 0;uc_i < 8;uc_i++)     //ѭ�����ո�λ����
    {
        I2C_SHT_SCK_0();                   //ʱ�ӵ�
        _DELAY(200);                        //��ʱ����
        I2C_SHT_SCK_1();                   //ʱ�Ӹ�
        _DELAY(150);                        //��ʱ����
        byte <<= 1;                         //����λ����
        if(I2C_SHT_GETSDA())               //��ȡ����λ
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
 * @����: i2c��������
 * 
 * @����: 
 * @����: 
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
void I2C_SHT_Start(void)
{
    I2C_SHT_SDA_1();                       //�����������
    I2C_SHT_SCK_1();                       //ʱ���������
    _DELAY(200);                            //��ʱ ����
    I2C_SHT_SDA_0();                       //�������½���
    _DELAY(200);                            //��ʱ ����
    I2C_SHT_SCK_0();                       //ʱ���������
    
}

/*******************************************************************************
 * @function_name:  I2C_SHT_Stop
 * @function_file:  IIC_lsensor.C
 * @����: I2C����ֹͣ
 * 
 * @����: 
 * @����: 
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
void I2C_SHT_Stop(void)
{
    //I2C_3231_SCK_0();                       //ʱ���������
    I2C_SHT_SDA_0();                       //�����������
    _DELAY(200);                            //��ʱ ����
    I2C_SHT_SCK_1();                       //ʱ���������
    _DELAY(200);                            //��ʱ ����
    I2C_SHT_SDA_1();                       //������������
    //_DELAY(100);                            //��ʱ ����
    //I2C_3231_SCK_0();                       //ʱ���������

}

/*******************************************************************************
 * @function_name:  I2C_SHT_ACK
 * @function_file:  IIC_lsensor.C
 * @����: I2C����ACKӦ���ź�
 * 
 * @����: 
 * @����: 
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
void I2C_SHT_ACK(void)
{
    
    I2C_SHT_SCK_0();                       //ʱ���ߵ͵�ƽ
    _DELAY(50);
    I2C_SHT_SDAOUT();
    _DELAY(100);
    I2C_SHT_SDA_0();                       //�����ߵ͵�ƽ
    _DELAY(100);                            //��ʱ ����
    I2C_SHT_SCK_1();                       //ʱ����������
    _DELAY(200);                            //��ʱ ����
    I2C_SHT_SCK_0();                       //ʱ�����½���
}

/*******************************************************************************
 * @function_name:  I2C_SHT_NAK
 * @function_file:  IIC_lsensor.C
 * @����: I2C����NAK�ź�
 * 
 * @����: 
 * @����: 
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
void I2C_SHT_NAK(void)
{
    I2C_SHT_SCK_0();                       //ʱ���ߵ͵�ƽ
    _DELAY(50);
    I2C_SHT_SDAOUT();
    _DELAY(100);
    I2C_SHT_SDA_1();                       //�����߸��ƽ
    _DELAY(100);                            //��ʱ ����
    I2C_SHT_SCK_1();                       //ʱ��������
    _DELAY(200);                            //��ʱ ����
    I2C_SHT_SCK_0();                       //ʱ�����½���
}



/*******************************************************************************
 * @function_name:  I2C_SHT_ReadLX
 * @function_file:  IIC_lsensor.C
 * @����: �⴫�������ݶ�ȡ
 * 
 * 
 * @����: 
 * @param: date  ���ݻ���
 * 
 * @����: 
 * @return:  uint8   
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
uint8 I2C_SHT_Read(uint8* date)
{

    I2C_SHT_Start();                       //I2C��������               
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);         //д��������
    I2C_SHT_WriteByte(0x00);               //д���ȡ���ݵĵ�ַ
    
    I2C_SHT_Stop();
    I2C_SHT_Stop();

    I2C_SHT_Start();                       //I2C������������
    I2C_SHT_WriteByte(I2C_SHT_RD_ID);     //��ȡ��������
    for(uint8 uc_i = 0; uc_i < 2; uc_i++)     //ѭ�������ֽ�����
    {
        date[uc_i] = I2C_SHT_ReadByte();   //��ȡһ���ֽ�����
        if(uc_i == 1)                       //���һ���ֽڷ���NAK
        {
            I2C_SHT_NAK();
        }
        else                                //����ACK
        {
            I2C_SHT_ACK();
        }
    }
    I2C_SHT_Stop();                        //I2C����ֹͣ
                                            //��ȡ����
//	    time[3] &= 0x7F;
//	    time[3] = STR_GetFirstSetBit(time + 3, 1);
    SYS_OK();
}


/*******************************************************************************
 * @function_name:  I2C_SHT_WriteBcdTime
 * @function_file:  IIC_lsensor.C
 * @����: �⴫��������д��
 * 
 * 
 * @����: 
 * @param: data  
 * 
 * @����: 
 * @return:  uint8   
 * @����: yzy (2019-11-03)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
uint8 I2C_SHT_Write(uint8 data)
{
    uint8 err = 1;                          //д����Ĭ��err

                                            //���д�벻�ɹ�����д,�����д3��
    for(uint8 uc_i = 0;(uc_i < 3) && (err != 0); uc_i++)
    {
        I2C_SHT_Start();                   //i2c��������
                                            //д����
        err = I2C_SHT_WriteByte(I2C_SHT_WR_ID);
        //err |= I2C_SHT_WriteByte(0x00);    //д�������ַ

//	        for(uint8 uc_j = 0;uc_j < 7;uc_j++) //ѭ��д������ֽ�
        {
          //_DELAY(50);
            err |= I2C_SHT_WriteByte(data);
        }
        I2C_SHT_Stop();                    //i2c����ֹͣ
    }
    return err;                             //���ؽ��
}


/*******************************************************************************
 * @function_name:  Init_I2C_LS
 * @function_file:  IIC_lsensor.C
 * @����: �⴫����I2C���߳�ʼ��
 * 
 * @����: 
 * @����: 
 * @����: yzy (2019-11-3)
 *-----------------------------------------------------------------------------
 * @�޸���: 
 * @�޸�˵��: 
 ******************************************************************************/
void Init_I2C_Sht(/*const IIC_PORTS* ports*/)
{
    uint8 uc_i;
//	      __HAL_RCC_GPIOB_CLK_ENABLE();
                                            //����SDA�˿ں�SCK�˿�
//	    HAL_GPIO_PinConfig(ports->portSDAOut->gpio, ports->portSDAOut->pinnum, ports->portSDAOut->modefunc, ports->portSDAOut->dir);
//	    HAL_GPIO_PinConfig(ports->portSCK->gpio, ports->portSCK->pinnum, ports->portSCK->modefunc, ports->portSCK->dir);
    
    stc_gpio_cfg_t stcGpioCfg;
    
    DDL_ZERO_STRUCT(stcGpioCfg);
    
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio,TRUE);   //����GPIOʱ���ſ� 
    
    stcGpioCfg.enDir = GpioDirOut;                           ///< �˿ڷ�������->���    
    stcGpioCfg.enOD = GpioOdEnable;                          ///< ��©���
    stcGpioCfg.enPu = GpioPuEnable;                          ///< �˿���������->ʹ��
    stcGpioCfg.enPd = GpioPdDisable;                         ///< �˿���������->��ֹ
        
    Gpio_Init(GpioPortB,GpioPin13,&stcGpioCfg);               ///< �˿ڳ�ʼ��
    Gpio_Init(GpioPortB,GpioPin14,&stcGpioCfg);
    //    

    //��ֵȫ�ֽṹ��
//	    gsp_LSIICPrt = ports;
//    I2C_SHT_SCK_0(); 
//    I2C_SHT_SDA_0(); 
    
    I2C_SHT_SDAIN();                       //�����ź���Ĭ������
    I2C_SHT_SCKOUT();                      //ʱ���ź���Ĭ�����
    I2C_SHT_SCK_1();                       //Ĭ������͵�ƽ

    for(uc_i = 0;uc_i < 10;uc_i++)          //ѭ�����������״̬,��ʱ������Ϊ����̬
    {
        _DELAY(150);
        if(I2C_SHT_GETSDA())               //�ߵ�ƽ,�����һ��Start�ź�
        {
            I2C_SHT_SDAOUT();
            I2C_SHT_SDA_1();
            break;
        }
                                            //�����ߵ͵�ƽ,���������,�任ʱ����(������),����ʱ��
        I2C_SHT_SCK_0();                   //ʱ�������
        _DELAY(200);                        //��ʱ����
        I2C_SHT_SCK_1();                   //ʱ�������(������)
        _DELAY(200);                        //��ʱ����
    }
    if(uc_i == 10)                          //10��ʱ��������,δ�ܽ�������ʱ��,����
    {
        return;
    }
    I2C_SHT_SCK_0();
    _DELAY(200);
    I2C_SHT_SCK_1();
    _DELAY(150);
    I2C_SHT_SDA_1();

    //I2C_3231_F32KOFF();                     //Ĭ�Ϲر�32K�ź����
   // I2C_SHT_SecIntOnOff(true);             //��ʼ�������ж����
}


