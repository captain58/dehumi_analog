#include "bsp_flash.h"

static uint32_t g_u32FlashAddr = 0xfC00;

void Bsp_Flash_Init(void) {
    ///< 确保初始化正确执行后方能进行FLASH编程操作，FLASH初始化（编程时间,休眠模式配置）
    while(Ok != Flash_Init(6, TRUE)) {
        rt_thread_mdelay(1);
    }
    
    Debug_Print("Flash Init Success!");

    return;
}

void CloseGlobalIrq(void) {
    __disable_irq();
}

void OpenGlobalIrq(void) {
    __enable_irq();
}

/* flash 读取数据 */
boolean_t Bsp_Flash_ReadData(uint8_t *pdata, uint16_t uiLen) {
    uint32_t uiAddr = g_u32FlashAddr;
    
    if (uiLen >= 1024) {
        Debug_Print("Bsp_Flash_ReadData failed! uiLen:%u", uiLen);
        return FALSE;
    }

    if ((*((volatile uint8_t*)(uiAddr)) != 0x12) ||
        (*((volatile uint8_t*)(uiAddr + 1)) != 0x34) ||
        (*((volatile uint8_t*)(uiAddr + 2)) != 0x56) ||
        (*((volatile uint8_t*)(uiAddr + 3)) != 0x78)) {
        Debug_Print("read magic failed!");
        return FALSE;
    }

    uiAddr += 4;
    for (int i = 0; i < uiLen; i++) {
        pdata[i] = *((volatile uint8_t*)uiAddr);
        uiAddr++;
    }

    return TRUE;
}

/* flash 写入数据 */
boolean_t Bsp_Flash_StoreData(uint8_t *pdata, uint16_t uiLen) {
    uint32_t uiAddr;
    en_result_t res; 

    if (uiLen >= 1024) {
        Debug_Print("Bsp_Flash_StoreData failed! uiLen:%u", uiLen);
        return FALSE;
    }

    ///< FLASH目标扇区擦除
    CloseGlobalIrq();
    uiAddr = g_u32FlashAddr;
    res = Flash_SectorErase(uiAddr);
    uiAddr += 512;
    res += Flash_SectorErase(uiAddr);
    if (res) {
        OpenGlobalIrq();
        Debug_Print("write magic failed!");
        return FALSE;
    }

    /* 写入数据 */
    uiAddr = g_u32FlashAddr + 4;
    for (int i = 0; i < uiLen; i++) {
        if ((Ok != Flash_WriteByte(uiAddr, pdata[i])) || (*((volatile uint8_t*)uiAddr) != pdata[i])) {
            OpenGlobalIrq();
            Debug_Print("write byte err! addr:0x%x, data:0x%x", uiAddr, pdata[i]);
            return FALSE;
        }
        uiAddr++;
    }
    
    uiAddr = g_u32FlashAddr;
    res = Flash_WriteByte(uiAddr++, 0x12);
    res += Flash_WriteByte(uiAddr++, 0x34);
    res += Flash_WriteByte(uiAddr++, 0x56);
    res += Flash_WriteByte(uiAddr++, 0x78);
    OpenGlobalIrq();
    if (res) {
        Debug_Print("write magic failed!");
        return FALSE;
    }
    Debug_Print("Bsp_Flash_StoreData Success! len:%u", uiLen);
    return TRUE;
}
