#ifndef __SRC_TEMP_H__
#define __SRC_TEMP_H__

#include "main.h"
#include "iodev.h"
#include "bsp/bsp_i2c.h"
#include "bsp/bsp_adc_ntc.h"



/* ADDR Pin Conect to VSS */
#define	SHT30_ADDR_WRITE	0x44<<1         //10001000
#define	SHT30_ADDR_READ		(0x44<<1)+1	    //10001011

typedef enum
{
    /* 软件复位命令 */
    SOFT_RESET_CMD = 0x30A2,	
    /*单次测量模式命名格式：Repeatability_CS_CMDCS： Clock stretching*/
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,
    /*周期测量模式命名格式：Repeatability_MPS_CMDMPS：measurement per second*/
    HIGH_0_5_CMD   = 0x2032,
    MEDIUM_0_5_CMD = 0x2024,
    LOW_0_5_CMD    = 0x202F,
    HIGH_1_CMD     = 0x2130,
    MEDIUM_1_CMD   = 0x2126,
    LOW_1_CMD      = 0x212D,
    HIGH_2_CMD     = 0x2236,
    MEDIUM_2_CMD   = 0x2220,
    LOW_2_CMD      = 0x222B,
    HIGH_4_CMD     = 0x2334,
    MEDIUM_4_CMD   = 0x2322,
    LOW_4_CMD      = 0x2329,
    HIGH_10_CMD    = 0x2737,
    MEDIUM_10_CMD  = 0x2721,
    LOW_10_CMD     = 0x272A,
    /* 周期测量模式读取数据命令 */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
}SHT30_CMD;

/**
* @brief	复位SHT30
* @param	none
* @retval	none
*/
void SHT30_reset(void);

/**
* @brief	初始化SHT30
* @param	none
* @retval	成功返回HAL_OK
* @note	周期测量模式
*/
uint8_t SHT30_Init(void);

/**
* @brief	从SHT30读取一次数据
* @param	dat —— 存储读取数据的地址（6个字节数组）
* @retval	成功 —— 返回HAL_OK
*/
uint8_t SHT30_Read_Dat(uint8_t* dat);

/*
*
* @brief	将SHT30接收的6个字节数据进行CRC校验，并转换为温度值和湿度值
* @param	dat  —— 存储接收数据的地址（6个字节数组）
* @retval	校验成功  —— 返回0
* 			校验失败  —— 返回1，并设置温度值和湿度值为0
*/
uint8_t SHT30_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity);


boolean_t Sync_TempHum(int16_t *pitemp, uint16_t *puiHum);

#endif
