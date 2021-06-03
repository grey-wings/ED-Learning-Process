#ifndef _FRAMEWORK_H_
#define _FRAMEWORK_H_

#include <stdint.h>

/****************************************************************************************/
/*                                                                                      */
/*                                       配置区                                         */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

/*
*配置区的参数（宏）需要用户根据自己的实际情况自行配置，如果有“!必须的!”这样的标记，则说明
*这个宏必须被合理配置；如果有“（可选的）”这样的标记，则说明该宏对应着可选功能，如果用户并
*不想使用该功能，就可以不配置。
*
*/

/*******************************
*!必须的!                      *
*这些宏指明你使用的器件类型    *
*这会影响到后面GPIO操作部分    *
********************************/
#define USE_STM32F7
//#define USE_STM32F4     //F4系列
//#define USE_STM32F1   //F1系列
/*******************************/


/**********************************
*!必须的！                        *
*包含对应的头文件（stm32xxxx.h）  *
***********************************/
#include "stm32f7xx.h"//你的器件对应的头文件
/**********************************/


/**************************************
*（可选的）                           *
*定义你的器件的最大主频               *
*对于F103系列这个值为72000000         *
*对于F407系列这个值为168000000        *
*对于F429/439系列这个值为180000000    *
*其他自己添加...                      *
***************************************/
//#define SYSCLK_MAX_CLOCK    48000000
//#define SYSCLK_MAX_CLOCK    72000000
//#define SYSCLK_MAX_CLOCK    168000000
//#define SYSCLK_MAX_CLOCK    180000000
#define SYSCLK_MAX_CLOCK    216000000
/**************************************/


/****************************************
*!必须的!                               *
*系统定时器的类型                       *
*系统定时器被用来提供一个公用的时钟基准 *
*它的作用是提供通用的延时函数和计时方法 *
*0：不使用系统定时器                    *
*1：systick定时器                       *
*2：用户自定义                          *
*****************************************/
#define SYSTEM_TIMER        1
/****************************************/


#if SYSTEM_TIMER!=0

/**************************************
*!必须的!                             *
*指定定时器的时钟频率对于systick定时  *
*器这个值通常是SYSCLK或SYSCLK/8       *
***************************************/
#define SYSTEM_TIMER_CLOCK  (SYSCLK_MAX_CLOCK>>3)
/**************************************/

#endif




/***********************************************************
*!必须的!                                                  *
*STM32中各种外设寄存器的定义                               *
*要使用的就把注释取消，如果没有你要用的外设可以自己添加进去*
************************************************************/
typedef union
{
    //ADC_TypeDef*                adc;
    //ADC_Common_TypeDef*         adc_common;
    //CAN_TxMailBox_TypeDef*      can_txmailbox;
    //CAN_FIFOMailBox_TypeDef*    can_fifomailbox;
    //CAN_FilterRegister_TypeDef* filterregister;
    //CAN_TypeDef*                can;
    //CRC_TypeDef*                crc;
    //DAC_TypeDef*                dac;
    //DBGMCU_TypeDef*             debugmcu;
    //DCMI_TypeDef*               dcmi;
    //DMA_Stream_TypeDef*         dma_stream;
    //DMA_TypeDef*                dma;
    //DMA2D_TypeDef*              dma2d;
    //ETH_TypeDef*                eth;
    //EXTI_TypeDef*               exti;
    //FLASH_TypeDef*              flash;
    //FMC_Bank1_TypeDef*          fmc_bank1;
    //FMC_Bank1E_TypeDef*         fmc_bank1e;
    //FMC_Bank2_3_TypeDef*        fmc_bank2_3;
    //FMC_Bank4_TypeDef*          fmc_bank4;
    //FMC_Bank5_6_TypeDef*        fmc_bank5_6;
    //GPIO_TypeDef*               gpio;
    //SYSCFG_TypeDef*             syscfg;
    //I2C_TypeDef*                i2c;
    //IWDG_TypeDef*               iwdg;
    //LTDC_TypeDef*               ltdc;
    //LTDC_Layer_TypeDef*         ltdc_layer;
    //PWR_TypeDef*                pwr;
    //RCC_TypeDef*                rcc;
    //RTC_TypeDef*                rtc;
    //SAI_TypeDef*                sai;
    //SAI_Block_TypeDef*          sai_block;
    //SDIO_TypeDef*               sdio;
//    SPI_TypeDef*                spi;
    //TIM_TypeDef*                tim;
    //USART_TypeDef*              usart;
    //WWDG_TypeDef*               wwdg;
    //RNG_TypeDef*                rng;
    //USB_OTG_GlobalTypeDef*      usa_otg_global;
    //USB_OTG_DeviceTypeDef*      usb_otg_device;
    //USB_OTG_INEndpointTypeDef*  usb_otg_inendpoint;
    //USB_OTG_OUTEndpointTypeDef* usb_otg_outendpoint;
    //USB_OTG_HostTypeDef*        usb_otg_host;
    //USB_OTG_HostChannelTypeDef* usb_otg_hostchannel;
} peripheral_register_type;
/**********************************************************/

/*====================================END=====================================*/













/****************************************************************************************/
/*                                                                                      */
/*                                       定义区                                         */
/*                                                                                      */
/****************************************************************************************/

#if !defined(USE_STM32F4) && !defined(USE_STM32F1) && !defined(USE_STM32F7) && !defined(USE_STM32H7)
#error No device has been selected!!!(没有定义任何器件）
#endif

/*======================================================================================*/

#define AHB_MAX_CLOCK       SYSCLK_MAX_CLOCK
#define APB1_MAX_CLOCK      (SYSCLK_MAX_CLOCK>>2)
#define APB2_MAX_CLOCK      (SYSCLK_MAX_CLOCK>>1)

/*
*为了增强代码可读性，定义几种可能会用到的数据类型
*1.sign：标志。
*        如果一个变量只起到标志的作用（只具有有限的几种状态），
*        那么应该有sign修饰，例如一个变量用来表示ADC是否读取完
*        毕，那么改变量就是一个标志位，它的值只有逻辑的意义，
*        没有实际大小的意义。
*2.reg：寄存器。
*       起到寄存器作用的变量用其修饰，例如该变量对应了外部芯片
*       中的某个实际的寄存器，就应该有reg修饰。
*3.ro：只读。
*      该变量只希望被用户读取，用户不应该改变其值。如果一个被
*      ro修饰，那么该变量在最初赋值以后（最好）在整个程序执行
*      过程都不会改变。
*4.vro：只读并且可能经常被_RO_WRITE方法改变
*/
#define sign    volatile        //标志类型
#define reg     volatile        //寄存器类型
#define ro      const           //只读类型
#define vro     volatile const  //vro

//只读类型的写入方法
//三个参数意义：对象、对象的数据类型、要写入的值
#define _RO_WRITE(object,type,value)	*((type*)((uint32_t)(&object)))=value



typedef void*   inst_type;  //实例类型，对应各种类的实例

/*
*外设接口类
*针对寄存器写法、标准库写法、HAL库写法的外设接口
*定义该类的目的在于让各种写法都能以统一方式访问外设
*/
typedef struct peripheral_interface_class
{
    //由用户自行决定其作用（如果有必要使用的话），当然也可以不使用
    ro uint32_t                    Value;

    //存放外设的寄存器地址，如：TIM1，USART1，SPI2等
    ro peripheral_register_type    Register0;

    //（备用的）存放外设的寄存器地址，如：TIM1，USART1，SPI2等
    //如果一个Register0不够用才可以用这个
    ro peripheral_register_type    Register1;

    //存放某些库中定义的结构体
    //如HAL库中的SPI_Handler等
    ro inst_type                   Instance;
} periperal;

//C语言基本数据类型
//在进行数据类型转换的时候，这些定义可能会有用
typedef enum
{
    UCHAR_Format,
    CHAR_Format,
    USHORT_Format,
    SHORT_Format,
    INT_Format,
    UINT_Format,
    FLOAT_Format,
    DOUBLE_Format
} data_format_type;

//硬件或软件
//指明一些外设功能的实现方法
//如硬件SPI或软件模拟SPI
typedef enum
{
    SOFTWARE,
    HARDWARE
} S_H_type;


/*========================================END===========================================*/










/****************************************************************************************/
/*                                                                                      */
/*                                       GPIO操作                                       */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

/*
*这部分代码参考了正点原子
*原理是利用Cortex内核的位带操作功能，实现GPIO的高效读/写
*所有操作使用宏实现，提高操作速度
*细节不加赘述，下面是使用方法
*
*
*针对F4/F1系列：
*1.GPIO的拉高/低(GPIO_Write())
*例：GPIO_Write(GPIOA_6)=1;该语句将PA6拉高
*例：GPIO_Write(GPIOI_15)=0;该语句将PI15拉低
*
*2.GPIO输入读取(GPIO_Read())
*例：GPIO_Read(GPIOB_0);该语句返回PB0上的输入值
*
*3.获取IO组中某个端口的偏移(GPIO_Get_Pin_Addr())
*例：GPIO_Get_Pin_Addr(GPIOB_3);该语句返回:1<<3
*例：GPIO_Get_Pin_Addr(GPIOA_12);该语句返回:1<<12
*
*4.获取IO组中某个端口的序号(GPIO_Get_Pin_Num())
*例：GPIO_Get_Pin_Num(GPIOA_3);该语句返回:3
*例：GPIO_Get_Pin_Num(GPIOA_12);该语句返回:12
*
*
*
*针对F7系列：
*1.GPIO的拉高/低(GPIO_Write())
*例：GPIO_Write(GPIOA_6,1);该语句将PA6拉高
*例：GPIO_Write(GPIOI_15,0);该语句将PI15拉低
*
*2.GPIO输入读取(GPIO_Read())
*例：GPIO_Read(GPIOB_0);该语句返回PB0上的输入值
*
*3.获取IO组中某个端口的偏移(GPIO_Get_Pin_Addr())
*例：GPIO_Get_Pin_Addr(GPIOB_3);该语句返回:1<<3
*例：GPIO_Get_Pin_Addr(GPIOA_12);该语句返回:1<<12
*/







//////////////////////////////////////////////////////////////////////////////////


//bit0~bit15：1<<Pinx
//bit16~bit19：GPIOA=0,GPIOB=1,GPIOC=2,...
//bit20~bit23：PIN0_ADDR=0,PIN1_ADDR=1,PIN3_ADDR=3,...

//GPIO引脚编号定义
typedef enum
{
    PIN0_ADDR=1<<0,
    PIN1_ADDR=1<<1,
    PIN2_ADDR=1<<2,
    PIN3_ADDR=1<<3,
    PIN4_ADDR=1<<4,
    PIN5_ADDR=1<<5,
    PIN6_ADDR=1<<6,
    PIN7_ADDR=1<<7,
    PIN8_ADDR=1<<8,
    PIN9_ADDR=1<<9,
    PIN10_ADDR=1<<10,
    PIN11_ADDR=1<<11,
    PIN12_ADDR=1<<12,
    PIN13_ADDR=1<<13,
    PIN14_ADDR=1<<14,
    PIN15_ADDR=1<<15
} pin_type;

typedef enum
{
    GPIO_GROUPA=0<<16,
    GPIO_GROUPB=1<<16,
    GPIO_GROUPC=2<<16,
    GPIO_GROUPD=3<<16,
    GPIO_GROUPE=4<<16,
    GPIO_GROUPF=5<<16,
    GPIO_GROUPG=6<<16,
    GPIO_GROUPH=7<<16,
    GPIO_GROUPI=8<<16
} gpio_group_type;

typedef enum
{
    PIN0_NUM=0<<20,
    PIN1_NUM=1<<20,
    PIN2_NUM=2<<20,
    PIN3_NUM=3<<20,
    PIN4_NUM=4<<20,
    PIN5_NUM=5<<20,
    PIN6_NUM=6<<20,
    PIN7_NUM=7<<20,
    PIN8_NUM=8<<20,
    PIN9_NUM=9<<20,
    PIN10_NUM=10<<20,
    PIN11_NUM=11<<20,
    PIN12_NUM=12<<20,
    PIN13_NUM=13<<20,
    PIN14_NUM=14<<20,
    PIN15_NUM=15<<20
} pin_num_type;

typedef enum
{
    GPIOA_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPA,
    GPIOA_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPA,
    GPIOA_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPA,
    GPIOA_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPA,
    GPIOA_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPA,
    GPIOA_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPA,
    GPIOA_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPA,
    GPIOA_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPA,
    GPIOA_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPA,
    GPIOA_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPA,
    GPIOA_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPA,
    GPIOA_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPA,
    GPIOA_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPA,
    GPIOA_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPA,
    GPIOA_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPA,
    GPIOA_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPA,

    GPIOB_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPB,
    GPIOB_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPB,
    GPIOB_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPB,
    GPIOB_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPB,
    GPIOB_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPB,
    GPIOB_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPB,
    GPIOB_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPB,
    GPIOB_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPB,
    GPIOB_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPB,
    GPIOB_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPB,
    GPIOB_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPB,
    GPIOB_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPB,
    GPIOB_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPB,
    GPIOB_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPB,
    GPIOB_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPB,
    GPIOB_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPB,

    GPIOC_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPC,
    GPIOC_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPC,
    GPIOC_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPC,
    GPIOC_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPC,
    GPIOC_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPC,
    GPIOC_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPC,
    GPIOC_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPC,
    GPIOC_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPC,
    GPIOC_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPC,
    GPIOC_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPC,
    GPIOC_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPC,
    GPIOC_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPC,
    GPIOC_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPC,
    GPIOC_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPC,
    GPIOC_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPC,
    GPIOC_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPC,

    GPIOD_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPD,
    GPIOD_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPD,
    GPIOD_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPD,
    GPIOD_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPD,
    GPIOD_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPD,
    GPIOD_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPD,
    GPIOD_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPD,
    GPIOD_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPD,
    GPIOD_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPD,
    GPIOD_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPD,
    GPIOD_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPD,
    GPIOD_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPD,
    GPIOD_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPD,
    GPIOD_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPD,
    GPIOD_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPD,
    GPIOD_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPD,

    GPIOE_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPE,
    GPIOE_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPE,
    GPIOE_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPE,
    GPIOE_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPE,
    GPIOE_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPE,
    GPIOE_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPE,
    GPIOE_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPE,
    GPIOE_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPE,
    GPIOE_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPE,
    GPIOE_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPE,
    GPIOE_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPE,
    GPIOE_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPE,
    GPIOE_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPE,
    GPIOE_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPE,
    GPIOE_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPE,
    GPIOE_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPE,

    GPIOF_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPF,
    GPIOF_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPF,
    GPIOF_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPF,
    GPIOF_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPF,
    GPIOF_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPF,
    GPIOF_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPF,
    GPIOF_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPF,
    GPIOF_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPF,
    GPIOF_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPF,
    GPIOF_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPF,
    GPIOF_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPF,
    GPIOF_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPF,
    GPIOF_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPF,
    GPIOF_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPF,
    GPIOF_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPF,
    GPIOF_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPF,

    GPIOG_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPG,
    GPIOG_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPG,
    GPIOG_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPG,
    GPIOG_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPG,
    GPIOG_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPG,
    GPIOG_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPG,
    GPIOG_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPG,
    GPIOG_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPG,
    GPIOG_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPG,
    GPIOG_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPG,
    GPIOG_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPG,
    GPIOG_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPG,
    GPIOG_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPG,
    GPIOG_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPG,
    GPIOG_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPG,
    GPIOG_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPG,

    GPIOH_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPH,
    GPIOH_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPH,
    GPIOH_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPH,
    GPIOH_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPH,
    GPIOH_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPH,
    GPIOH_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPH,
    GPIOH_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPH,
    GPIOH_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPH,
    GPIOH_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPH,
    GPIOH_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPH,
    GPIOH_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPH,
    GPIOH_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPH,
    GPIOH_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPH,
    GPIOH_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPH,
    GPIOH_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPH,
    GPIOH_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPH,

    GPIOI_0=PIN0_ADDR+PIN0_NUM+GPIO_GROUPI,
    GPIOI_1=PIN1_ADDR+PIN1_NUM+GPIO_GROUPI,
    GPIOI_2=PIN2_ADDR+PIN2_NUM+GPIO_GROUPI,
    GPIOI_3=PIN3_ADDR+PIN3_NUM+GPIO_GROUPI,
    GPIOI_4=PIN4_ADDR+PIN4_NUM+GPIO_GROUPI,
    GPIOI_5=PIN5_ADDR+PIN5_NUM+GPIO_GROUPI,
    GPIOI_6=PIN6_ADDR+PIN6_NUM+GPIO_GROUPI,
    GPIOI_7=PIN7_ADDR+PIN7_NUM+GPIO_GROUPI,
    GPIOI_8=PIN8_ADDR+PIN8_NUM+GPIO_GROUPI,
    GPIOI_9=PIN9_ADDR+PIN9_NUM+GPIO_GROUPI,
    GPIOI_10=PIN10_ADDR+PIN10_NUM+GPIO_GROUPI,
    GPIOI_11=PIN11_ADDR+PIN11_NUM+GPIO_GROUPI,
    GPIOI_12=PIN12_ADDR+PIN12_NUM+GPIO_GROUPI,
    GPIOI_13=PIN13_ADDR+PIN13_NUM+GPIO_GROUPI,
    GPIOI_14=PIN14_ADDR+PIN14_NUM+GPIO_GROUPI,
    GPIOI_15=PIN15_ADDR+PIN15_NUM+GPIO_GROUPI,

    GPIO_NONE
} gpio_type;

#define GPIO_Get_Pin_Num(x)		(x>>20)
#define _Get_GPIO_Num(x)		((x>>16)&0xf)

#define GPIO_Get_Pin_Addr(x)    (x&0xffff)
#define _Get_GPIO_Addr(x)	    ((GPIO_TypeDef*)((_Get_GPIO_Num(x)<<10)+GPIOA_BASE))




#if defined USE_STM32F4 || defined USE_STM32F1
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) 	((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  		*((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)  MEM_ADDR(BITBAND(addr, bitnum))

//ODR或IDR地址偏移量
#if defined USE_STM32F4
#define ODR_OFFSET 20
#define IDR_OFFSET 16
#elif defined USE_STM32F1
#define ODR_OFFSET 12
#define IDR_OFFSET 8
#endif

#define GPIO_Write(x)		BIT_ADDR(((uint32_t)_Get_GPIO_Addr(x)+ODR_OFFSET),GPIO_Get_Pin_Num(x))
#define GPIO_Read(x)		BIT_ADDR(((uint32_t)_Get_GPIO_Addr(x)+IDR_OFFSET),GPIO_Get_Pin_Num(x))
#endif




#if defined USE_STM32F7

#define BSRR_OFFSET         0x18
#define IDR_OFFSET          0x10

//x：GPIO端口，y：要写入的值（0/1）
#define GPIO_Write(x,y)      *((uint16_t*)(_Get_GPIO_Addr(x)+BSRR_OFFSET)+((~(y))&1))|=GPIO_Get_Pin_Addr(x)
#define GPIO_Read(x)         ((*((uint32_t*)(_Get_GPIO_Addr(x)+IDR_OFFSET)>>GPIO_Get_Pin_Num(x))&1)

#endif
/*=========================================END==========================================*/










/****************************************************************************************/
/*                                                                                      */
/*                                     系统定时器                                       */
/*                                                                                      */
/****************************************************************************************/

/*======================================================================================*/

/*
*系统定时器是一个公用的定时器，为整个系统提供通用的、简单的、标准的倒计时或延时功能
*任何一个库的代码都可以使用该定时器，并且该定时器可以在不同的库中被复用（不用再为每个器件单独分配一个外设定时器了）
*默认情况下该定时器是SysTick定时器（推荐），但是用户也可以指配为其他定时器
*当系统定时器被指定为SysTick定时器时，用户不需要为其编写任何驱动代码，而可以直接使用
*但是要注意不同器件中SysTick的时钟频率可能不同，在配置区要为系统定时器提供正确的时钟频率
*否则会导致定时时间不准
*
*倒计时功能演示：
*
*SystemTimer_Timing_Ready(1000);   //系统定时器准备，计时时间为：x(us)
*SystemTimer_Timing_Begin();    //开始倒计时
*for (i=0;i<10;i++)
*{
*   Process();                     //执行一些操作
*   while (SystemTimer_Timing_Check()==0);  //等待倒计时结束
*}
*SystemTimer_Timing_End();  //计时结束
*
*执行上面程序的结果是：Process()被执行了10次，而且每1000us才执行一次
*/








//如果使用系统定时器
#if SYSTEM_TIMER!=0

//us级延时
void SystemTimer_Delay_Us(uint32_t time);

//ms级延时
void SystemTimer_Delay_Ms(uint32_t time);

//s级延时
void SystemTimer_Delay_S(uint32_t time);

//如果使用SysTick定时器
#if SYSTEM_TIMER==1

//定时器复位（使能位、溢出标志位和计数器清零）
#define SystemTimer_Reset()                 SysTick->CTRL=0; \
                                            SysTick->VAL=0

//开始倒计时
#define SystemTimer_Timing_Begin()          SysTick->CTRL|=1;

//检查倒计时结束标志位
#define SystemTimer_Timing_Check()          ((SysTick->CTRL>>16)&1)

//结束倒计时
#define SystemTimer_Timing_End()            SystemTimer_Reset()

void SystemTimer_Timing_Ready(float time_us);

#elif SYSTEM_TIMER==2

void SystemTimer_Timing_Ready(uint32_t time_us);

void SystemTimer_Reset();
                  
void SystemTimer_Timing_Begin();

uint8_t SystemTimer_Timing_Check();

void SystemTimer_Timing_End();

void SystemTimer_Reset(void);

#endif

#endif

/*===========================================END========================================*/





#endif
