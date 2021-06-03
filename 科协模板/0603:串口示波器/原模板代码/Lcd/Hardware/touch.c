#include "touch.h"
#include "tftlcd.h"
#include "delay.h"
#include "stdlib.h"
#include "math.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//触摸屏驱动（支持ADS7843/7846/UH7843/7846/XPT2046/TSC2046/OTT2001A等） 代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/30
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//无
//////////////////////////////////////////////////////////////////////////////////

_m_tp_dev tp_dev=
        {
                TP_Init,
                GT9147_Scan,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
                0,
        };
//默认为touch_type=0的数据.
u8 CMD_RDX=0XD0;
u8 CMD_RDY=0X90;

//触摸屏初始化
//返回值:0,没有进行校准
//       1,进行过校准
u8 TP_Init(void)
{
    //0x5510
    if(GT9147_Init()==0)	//是GT9147
    {
        tp_dev.scan=GT9147_Scan;	//扫描函数指向GT9147触摸屏扫描
    }
    tp_dev.touchtype|=0X80;	//电容屏
    tp_dev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏
    return 0;
}


