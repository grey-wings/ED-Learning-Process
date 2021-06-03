//
// Created by QHJSTC on 2021/4/25.
//

#ifndef BASEPROJECT_LED_H
#define BASEPROJECT_LED_H

#include "Framework.h"
#include "tftlcd.h"

/*=====================================================================================================================*/
typedef struct led_class{
    /*===============================成员变量====================================*/
    GPIO_TypeDef*       Port;         // GPIO口
    uint16_t            Pin;          // Pin引脚

    /*===============================成员函数====================================*/
    void (*On)      (struct led_class* device);
    void (*Off)     (struct led_class* device);
    void (*Toggle)  (struct led_class* device);
}led;

/*=====================================================================================================================*/
void New_LED(led *device, GPIO_TypeDef* Port, uint16_t Pin);


#endif //BASEPROJECT_LED_H

