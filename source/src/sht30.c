#include "src/temp.h"

/**
* @brief	向SHT30发送一条指令(16bit)
* @param	cmd —— SHT30指令（在SHT30_MODE中枚举定义）
* @retval	成功返回HAL_OK
*/
static uint8_t	SHT30_Send_Cmd(SHT30_CMD cmd)
{
    uint8_t cmd_buffer[2];
    cmd_buffer[0] = cmd >> 8;
    cmd_buffer[1] = cmd;
    return Bsp_I2c_WriteData((uint8_t*)cmd_buffer, 2);
}

/**
* @brief	复位SHT30
* @param	none
* @retval	none
*/
void SHT30_reset(void)
{
    SHT30_Send_Cmd(SOFT_RESET_CMD);
    rt_thread_mdelay(20);
}

/**
* @brief	初始化SHT30
* @param	none
* @retval	成功返回HAL_OK
* @note	周期测量模式
*/
uint8_t SHT30_Init(void)
{
    return SHT30_Send_Cmd(MEDIUM_2_CMD);
}

/**
* @brief	从SHT30读取一次数据
* @param	dat —— 存储读取数据的地址（6个字节数组）
* @retval	成功 —— 返回HAL_OK
*/
uint8_t SHT30_Read_Dat(uint8_t* dat)
{
    SHT30_Send_Cmd(READOUT_FOR_PERIODIC_MODE);
    return Bsp_I2c_ReadData(dat, 6, 1000);
}

#define CRC8_POLYNOMIAL 0x31
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;	    //余数
    uint8_t  i = 0, j = 0;  //循环变量
    /* 初始化 */
    remainder = initial_value;
    for(j = 0; j < 2;j++){
        remainder ^= message[j];/* 从最高位开始依次计算  */
        for (i = 0; i < 8; i++){
            if (remainder & 0x80){
                remainder = (remainder << 1)^CRC8_POLYNOMIAL;
            }else{
                remainder = (remainder << 1);
            }
        }
    }
    /* 返回计算的CRC码 */
    return remainder;
}

/*
*
* @brief	将SHT30接收的6个字节数据进行CRC校验，并转换为温度值和湿度值
* @param	dat  —— 存储接收数据的地址（6个字节数组）
* @retval	校验成功  —— 返回0
* 			校验失败  —— 返回1，并设置温度值和湿度值为0
*/
uint8_t SHT30_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity)
{
    uint16_t recv_temperature = 0;
    uint16_t recv_humidity = 0;
    
    /* 校验温度数据和湿度数据是否接收正确 */
    if(CheckCrc8(dat, 0xFF) != dat[2] || CheckCrc8(&dat[3], 0xFF) != dat[5])
        return 1;
    /* 转换温度数据 */
    recv_temperature = ((uint16_t)dat[0]<<8)|dat[1];
    *temperature = -45 + 175*((float)recv_temperature/65535);
    /* 转换湿度数据 */
    recv_humidity = ((uint16_t)dat[3]<<8)|dat[4];
    *humidity = 100 * ((float)recv_humidity / 65535);
    return 0;
}


/* 获取温度，湿度  温度单位0.01°，湿度单位 %*100 */
boolean_t Sync_TempHum(int16_t *piTemp, uint16_t *puiHumi) {
    float temp;
    float humi;
    uint8_t szRecv[6] = {0};
    uint8_t uiFailed = 2;
    
    while (uiFailed--)
    {
        memset(szRecv, 0, sizeof(szRecv));

        if (TRUE != SHT30_Read_Dat(szRecv)) {
            Debug_Print("Sync_TempHum read failed!");
            rt_thread_mdelay(200);
            SHT30_reset();
            rt_thread_mdelay(20);
            if (TRUE == SHT30_Init()) {
                Debug_Print("sht30 init success.");
            } else {
                Debug_Print("sht30 init failed.");
            }
            rt_thread_mdelay(20);
            continue;
        }

        if (SHT30_Dat_To_Float(szRecv, &temp, &humi)) {
            Debug_Print("Sync_TempHum crc failed!");
            rt_thread_mdelay(200);
            SHT30_reset();
            rt_thread_mdelay(20);
            if (TRUE == SHT30_Init()) {
                Debug_Print("sht30 init success.");
            } else {
                Debug_Print("sht30 init failed.");
            }
            rt_thread_mdelay(20);
            continue;
        }

        break;
    }
    
    if ((uiFailed > 0) && (uiFailed <= 2)) {
        *piTemp = (int16_t)(temp * 100);
        *puiHumi = (uint16_t)(humi * 100);
        return TRUE;
    }

    return FALSE;
}

int Sht3x_Pro_Init(void) {
    SHT30_reset();
    if (TRUE == SHT30_Init()) {
        Debug_Print("sht30 init success.");
    } else {
        Debug_Print("sht30 init failed.");
    }

    return 0;
}

INIT_APP_EXPORT(Sht3x_Pro_Init);
