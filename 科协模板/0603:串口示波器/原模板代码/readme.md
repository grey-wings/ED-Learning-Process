# STM32配合Stm32Cube软件库
可以配合STM32多种芯片型号使用，目前支持（STM32 F4， STM32 F7）

## Description
软件库由多个外设组成，TFTLCD, LED, PLL, ADC

## Usage
###1、LCD介绍
- `lcd.c`
  - 主要用到三个函数
    - (1)Init();
       - 函数可以由用户自己修改，函数中需要放入LCD外设初始化，触摸初始化。
    - (4)Run();
       - 函数可以由用户自己修改，LCD运行函数里面放着一个LCD刷新屏幕，扫描按键，实施按键函数等操作。 
    - (2)Show_Init();
    - (3)Refresh_Button();
    
  - 不同外设需要修改的函数
    







