//
// Created by QHJSTC on 2021/5/7.
//

#include "AD9235x.h"
struct ad9235_class test;

/**************************SPI(与项目无关)*****************************/
//void Method_AD9235_Spi_Write_Byte(uint8_t dat){
//    HAL_SPI_Transmit(&hspi1, &dat, 1, 1000);
//}
//
//uint8_t Method_AD9235_Spi_Read_Byte(){
//    uint8_t rx_dat = 0;
//    rx_dat = HAL_SPI_Receive(&hspi1, &rx_dat, 1, 1000);
//    return rx_dat;
//}


/***************************函数定义***********************************/
void Method_Get_Data(struct ad9235_class* device, void* dat_buf, uint32_t num){

}

void Mehtod_Get_One_Data(struct ad9235_class device){

}

/**************************外部函数************************************/
ad9235 New_AD9235(void      (*spi_wt_byt) (uint8_t),
                  uint8_t   (*spi_rd_byt) (),
                  void      (*dly_us)     (uint32_t)
){
    ad9235 dev;
    dev = &test;
    dev->SPI_Write_Byte = spi_wt_byt;
    dev->SPI_Read_Byte = spi_rd_byt;
    dev->Get_Data = Method_Get_Data;
    dev->Delay_Us = dly_us;
    return dev;
}




















