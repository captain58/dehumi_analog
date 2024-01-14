#include "bsp_adc_ntc.h"
#include "main.h"

static uint32_t g_u32AdcRestult;
static uint8_t g_u32AdcStatus;
#ifdef BSP_ADC_NTC_USE_10K
static const uint32_t g_szNtcTempValue[151] = {
1792666,
1684053,
1582726,
1488151,
1399837,
1317332,
1240216,
1168107,
1100648,
1037512,
978396,
923020,
871124,
822471,
776837,
734018,
693823,
656077,
620616,
587288,
555953,
526480,
498747,
472643,
448062,
424906,
403086,
382516,
363117,
344817,
327547,
311243,
295847,
281301,
267556,
254562,
242274,
230650,
219650,
209239,
199380,
190041,
181193,
172807,
164857,
157317,
150164,
143376,
136933,
130816,
125005,
119485,
114239,
109252,
104510,
100000,
95709,
91626,
87738,
84037,
80512,
77154,
73953,
70903,
67995,
65221,
62576,
60051,
57642,
55342,
53146,
51049,
49045,
47130,
45300,
43551,
41878,
40278,
38748,
37283,
35882,
34540,
33255,
32025,
30846,
29717,
28635,
27597,
26603,
25649,
24734,
23856,
23014,
22206,
21431,
20686,
19970,
19283,
18623,
17989,
17380,
16794,
16231,
15689,
15168,
14667,
14185,
13722,
13275,
12845,
12431,
12033,
11649,
11279,
10923,
10580,
10249,
9930,
9623,
9326,
9040,
8764,
8498,
8241,
7994,
7754,
7523,
7300,
7085,
6877,
6676,
6482,
6295,
6113,
5938,
5769,
5605,
5447,
5293,
5145,
5002,
4863,
4729,
4599,
4474,
4352,
4234,
4120,
4009,
3902,
3799
};
#else
static const uint32_t g_szNtcTempValue[161] = {
1076457,
1015739,
958854,
905535,
855537,
808631,
764607,
723269,
684436,
647942,
613631,
581358,
550991,
522404,
495483,
470121,
446217,
423680,
402422,
382364,
363430,
345551,
328662,
312703,
297616,
283349,
269853,
257081,
244990,
233541,
222695,
212417,
202675,
193437,
184674,
176360,
168469,
160976,
153860,
147100,
140675,
134568,
128761,
123237,
117982,
112980,
108219,
103684,
99365,
95250,
91328,
87589,
84024,
80623,
77379,
74282,
71326,
68504,
65809,
63234,
60773,
58422,
56173,
54024,
51967,
50000,
48117,
46316,
44590,
42939,
41356,
39840,
38388,
36996,
35661,
34381,
33154,
31977,
30847,
29763,
28722,
27723,
26763,
25842,
24957,
24106,
23288,
22503,
21747,
21021,
20322,
19650,
19003,
18381,
17782,
17205,
16650,
16115,
15600,
15104,
14626,
14165,
13721,
13293,
12880,
12482,
12098,
11727,
11370,
11025,
10692,
10371,
10061,
9761,
9472,
9193,
8923,
8662,
8410,
8166,
7931,
7703,
7483,
7270,
7064,
6865,
6672,
6486,
6305,
6131,
5961,
5798,
5639,
5486,
5337,
5193,
5053,
4918,
4787,
4660,
4537,
4418,
4302,
4190,
4081,
3976,
3873,
3774,
3677,
3584,
3493,
3405,
3320,
3237,
3156,
3078,
3002,
2928,
2856,
2786,
2719
};
#endif

 ///< ADC中断服务函数
void Adc_IRQHandler(void)
{    
    rt_interrupt_enter();          //在中断中一定要调用这对函数，进入中断
    if(TRUE == Adc_GetIrqStatus(AdcMskIrqSgl))
    {
        Adc_ClrIrqStatus(AdcMskIrqSgl);
     
        g_u32AdcRestult = Adc_GetSglResult();
        g_u32AdcStatus = 1;
        Adc_SGL_Stop();
    }
    rt_interrupt_leave();    //在中断中一定要调用这对函数，离开中断
}

///< ADC采样端口初始化
void App_AdcPortInit(void)
{    
    ///< 开启ADC/BGR GPIO外设时钟
    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);
    
    Gpio_SetAnalogMode(BSP_ADC_NTC_GPIO, BSP_ADC_NTC_PIN);        //PA00 (AIN0)
    
    stc_gpio_cfg_t stcGpioCfg;

    DDL_ZERO_STRUCT(stcGpioCfg);

    ///<TX
    stcGpioCfg.enDir =     GpioDirIn;
    stcGpioCfg.enPu = GpioPuDisable;
    stcGpioCfg.enPd = GpioPdEnable;
    Gpio_Init(BSP_ADC_TEMP_GPIO, BSP_ADC_TEMP_PIN, &stcGpioCfg);
    Gpio_Init(BSP_ADC_HUMP_GPIO, BSP_ADC_HUMP_PIN, &stcGpioCfg);
    
    Gpio_SetAnalogMode(BSP_ADC_TEMP_GPIO, BSP_ADC_TEMP_PIN);        //PB10 (AIN17)
    Gpio_SetAnalogMode(BSP_ADC_HUMP_GPIO, BSP_ADC_HUMP_PIN);        //PB11 (AIN18)
}

///< ADC模块初始化
void App_AdcInit(void)
{
    stc_adc_cfg_t              stcAdcCfg;

    DDL_ZERO_STRUCT(stcAdcCfg);
    Sysctrl_SetPeripheralGate(SysctrlPeripheralAdcBgr, TRUE); 
    Bgr_BgrEnable();        ///< 开启BGR
    ///< ADC 初始化配置
    stcAdcCfg.enAdcMode         = AdcSglMode;               ///<采样模式-单次
    stcAdcCfg.enAdcClkDiv       = AdcMskClkDiv1;            ///<采样分频-1
    stcAdcCfg.enAdcSampCycleSel = AdcMskSampCycle12Clk;     ///<采样周期数-12
    stcAdcCfg.enAdcRefVolSel    = AdcMskRefVolSelAVDD;      ///<参考电压选择-外部电压
    stcAdcCfg.enAdcOpBuf        = AdcMskBufDisable;         ///<OP BUF配置-关
    stcAdcCfg.enInRef           = AdcMskInRefEnable;        ///<内部参考电压使能-开
    stcAdcCfg.enAdcAlign        = AdcAlignRight;            ///<转换结果对齐方式-右
    Adc_Init(&stcAdcCfg);
}

///< ADC 单次扫描模式 配置
void App_AdcSglCfg(en_adc_samp_ch_sel_t enstcAdcSampCh)
{
    ///< ADC 采样通道配置
    Adc_CfgSglChannel(enstcAdcSampCh);
    
    ///< ADC 中断使能
    Adc_EnableIrq();
    EnableNvic(ADC_IRQn, IrqLevel3, TRUE);
    
    ///< 启动单次转换采样
    Adc_SGL_Start();   

}

/* 通过当前电阻获取当前温度，单位0.01° */
int16_t Bsp_Adc_GetTemp(uint32_t uiOum) {
    int index;
    int idot;
    int iTemp;

    for (index = 0; index < sizeof(g_szNtcTempValue); index++) {
        if (g_szNtcTempValue[index] <= uiOum)
            break;
    }

    idot = (uiOum - g_szNtcTempValue[index]) * 100 / (g_szNtcTempValue[index-1] - g_szNtcTempValue[index]);
    #ifdef BSP_ADC_NTC_USE_10K
    iTemp = (index - 30) * 100 - idot;
    #else
    iTemp = (index - 40) * 100 - idot;
    #endif
    return iTemp;
}

/* 通过当前电压获取当前电阻,单位 0.1欧 */
uint32_t Bsp_Adc_GetOuM(int ver) {
    return (uint32_t)(500000000/ver - 100000);
}

/* 获取当前点电压，单位mV */
int Bsp_Adc_GetVer(en_adc_samp_ch_sel_t enstcAdcSampCh) {
    g_u32AdcStatus = 0;
    App_AdcSglCfg(enstcAdcSampCh);
    // App_AdcSglCfg(BSP_ADC_NTC_CHANAL);

    int failedcnt = 5;
    while (failedcnt--) {
        rt_thread_mdelay(50);
        if (g_u32AdcStatus) {
            break;
        }
    }
    
    if (g_u32AdcStatus) {
        // Debug_Print("adc success! data:%u\n", g_u32AdcRestult);
        return (g_u32AdcRestult * 5000 / 4096);
    } else {
        Debug_Print("adc failed!\n");
        return -1;
    }
}

/* 获取当前点adc值 */
int Bsp_Adc_GetAdc(en_adc_samp_ch_sel_t enstcAdcSampCh) {
    g_u32AdcStatus = 0;
    App_AdcSglCfg(enstcAdcSampCh);

    int failedcnt = 5;
    while (failedcnt--) {
        delay10us(1);
        if (g_u32AdcStatus) {
            break;
        }
    }
    
    if (g_u32AdcStatus) {
        return g_u32AdcRestult;
    } else {
        return 0;
    }
}

boolean_t Bsp_Adc_Start(int16_t *piTemp) {
    int iVer;
    uint32_t uiOum;
    int16_t iTemp;
    
    iVer = Bsp_Adc_GetVer(BSP_ADC_NTC_CHANAL);
    if (iVer == -1) {
        return FALSE;
    }

    uiOum = Bsp_Adc_GetOuM(iVer);
    // Debug_Print("adc success! oum:%u\n", uiOum);
    *piTemp = Bsp_Adc_GetTemp(uiOum);

    return TRUE;    
}

static int Bsp_Adc_Init(void) {
    ///< ADC 采样端口初始化
    App_AdcPortInit();    
    
    ///< ADC模块初始化
    App_AdcInit();

    return 0;
}

INIT_BOARD_EXPORT(Bsp_Adc_Init);
