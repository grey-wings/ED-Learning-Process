//
// Created by 15594 on 2021/2/11.
//

#ifndef OLED_TRY1_ZJH_OLED_H
#define OLED_TRY1_ZJH_OLED_H

#ifndef __OLED_H
#define __OLED_H

#include "sys.h"
#include "stdlib.h"
#include "main.h"
#include "oledfont.h"

//-----------------OLED端口定义----------------
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_RESET)//SCL(D0) 串行时钟线
#define OLED_SCLK_Set() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_4,GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET)//DIN(D1) 串行数据线 MOSI
#define OLED_SDIN_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET)

#define OLED_RES_Clr() HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_RESET)//RES 硬复位OLED
#define OLED_RES_Set() HAL_GPIO_WritePin(GPIOH,GPIO_PIN_11,GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)//DC 命令/数据标志（写0—命令/写1—数据）
#define OLED_DC_Set() HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)

#define OLED_CS_Clr()  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_RESET)//CS OLED片选信号
#define OLED_CS_Set()  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_6,GPIO_PIN_SET)

#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据
#define u8 unsigned char
#define u32 unsigned int

#endif

#endif //OLED_TRY1_ZJH_OLED_H
