/********************************************************
 * AD9235与FPGA通信 (SPI: CPOL = 1, CPHA = 1)
 *
 *******************************************************/

#ifndef BASEPROJECT_AD9235X_H
#define BASEPROJECT_AD9235X_H

#include "Framework.h"

struct ad9235_class{
    /*===========================成员变量=========================*/
    volatile uint32_t ad9235_clk;               // ad9235的时钟
    volatile uint32_t ad9235_mode;              // ad9235的模式

    /*===========================方法============================*/

    /**************************************************************
     *
     *************************************************************/
    void 	(*Get_Data)				   (struct ad9235_class* device, void* dat_buf, uint32_t num);

    /**************************************************************
     * 功能: SPI写一个字节
     * 参数: 1.dat: 要写的数据
     * 说明: CPHA = 1, CPOL = 1;
     *************************************************************/
    void    (*SPI_Write_Byte)           (uint8_t dat);


    /************************************************************
     * 功能: SPI接收一个字节
     * 参数: 无
     * 说明: CPHA = 1, CPOL = 1;
     *************************************************************/
    uint8_t (*SPI_Read_Byte)            (void);


    /*************************************************************
     *功能: 微妙延时
     *参数: 1.time: 延时的为描述
     *说明: 至少能够延时40ms
    **************************************************************/
    void (*Delay_Us)                  (uint32_t time);


};

// 类定义
typedef struct ad9235_class* ad9235;

// 创建一个ad9235, 并且返回一个ad9235
ad9235 New_AD9235();

/**************************spi****************************
 * 每个人根据自己的不同需求, 自己设定SPI的发送和接收函数
 ********************************************************/
void Method_AD9235_Spi_Write_Byte(uint8_t dat);
uint8_t Method_AD9235_Spi_Read_Byte();

#endif //BASEPROJECT_AD9235X_H

