//
// 一个Led简单的构造函数
//

#include "led.h"

/*************函数定义*****************/
void Method_Led_On(led* device);
void Method_Led_Off(led* device);
void Method_Led_Toggle(led* device);

//void test(){
//    printf("hello");
//}

// 打开Led
void Method_Led_On(led* device){
    HAL_GPIO_WritePin(device->Port, device->Pin, GPIO_PIN_SET);
}

// 关闭Led
void Method_Led_Off(led* device){
    HAL_GPIO_WritePin(device->Port, device->Pin, GPIO_PIN_RESET);
}

// Led翻转电平
void Method_Led_Toggle(led* device){
    HAL_GPIO_TogglePin(device->Port, device->Pin);
}

/****************************************/
// 创建一个新的LED类
void New_LED(led *device, GPIO_TypeDef* Port, uint16_t Pin){
    // 初始化Led的成员变量
    device->Port = Port;
    device->Pin = Pin;

    // 初始化Led的函数
    device->On = Method_Led_On;
    device->Off = Method_Led_Off;
    device->Toggle = Method_Led_Toggle;
    printf("成功创建一个LED!\r\n");
}



