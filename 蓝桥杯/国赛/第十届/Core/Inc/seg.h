#ifndef __SEG_H__
#define __SEG_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#define SER_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET)
#define SER_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET)
#define RCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_SET)
#define RCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET)
#define SCK_H HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_SET)
#define SCK_L HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET)

void seg_show(uint8_t num1,uint8_t num2,uint8_t num3);
	
#ifdef __cplusplus
}
#endif
#endif 