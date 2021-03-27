#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F7������
//ϵͳʱ�ӳ�ʼ��	
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/6/10
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//��
////////////////////////////////////////////////////////////////////////////////// 

//ʹ��CPU��L1-Cache
void Cache_Enable(void)
{
    SCB_EnableICache();//ʹ��I-Cache
    SCB_EnableDCache();//ʹ��D-Cache 
	SCB->CACR|=1<<2;   //ǿ��D-Cache͸д,�粻����,ʵ��ʹ���п���������������	
}
//ʱ�����ú���
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCOƵ��
//Fsys:ϵͳʱ��Ƶ��
//Fusb:USB,SDIO,RNG�ȵ�ʱ��Ƶ��
//Fs:PLL����ʱ��Ƶ��,������HSI,HSE��. 
//plln:��PLL��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:64~432.
//pllm:��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2,4,6,8.(������4��ֵ!)
//pllq:USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~15.

//�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=432,pllm=25,pllp=2,pllq=9.
//�õ�:Fvco=25*(432/25)=432Mhz
//     Fsys=432/2=216Mhz
//     Fusb=432/9=48Mhz
//����ֵ:0,�ɹ�;1,ʧ��
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
	
    __HAL_RCC_PWR_CLK_ENABLE(); //ʹ��PWRʱ��
 
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//���õ�ѹ�������ѹ�����Ա�������δ�����Ƶ�ʹ���
      
    RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;    //ʱ��ԴΪHSE
    RCC_OscInitStructure.HSEState=RCC_HSE_ON;                      //��HSE
    RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;				   //��PLL
    RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;          //PLLʱ��Դѡ��HSE
    RCC_OscInitStructure.PLL.PLLM=pllm;	//��PLL����ƵPLL��Ƶϵ��(PLL֮ǰ�ķ�Ƶ)
    RCC_OscInitStructure.PLL.PLLN=plln; //��PLL��Ƶϵ��(PLL��Ƶ)
    RCC_OscInitStructure.PLL.PLLP=pllp; //ϵͳʱ�ӵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ)
    RCC_OscInitStructure.PLL.PLLQ=pllq; //USB/SDIO/������������ȵ���PLL��Ƶϵ��(PLL֮��ķ�Ƶ)
    ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//��ʼ��
    if(ret!=HAL_OK) while(1);
    
    ret=HAL_PWREx_EnableOverDrive(); //����Over-Driver����
    if(ret!=HAL_OK) while(1);
    
    //ѡ��PLL��Ϊϵͳʱ��Դ��������HCLK,PCLK1��PCLK2
    RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;//����ϵͳʱ��ʱ��ԴΪPLL
    RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;//AHB��Ƶϵ��Ϊ1
    RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4;//APB1��Ƶϵ��Ϊ4
    RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2;//APB2��Ƶϵ��Ϊ2
    
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_7);//ͬʱ����FLASH��ʱ����Ϊ7WS��Ҳ����8��CPU���ڡ�
    if(ret!=HAL_OK) while(1);
}


#ifdef  USE_FULL_ASSERT
//��������ʾ�����ʱ��˺����������������ļ���������
//file��ָ��Դ�ļ�
//line��ָ�����ļ��е�����
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif

//�ж�I_Cache�Ƿ��
//����ֵ:0 �رգ�1 ��
u8 Get_ICahceSta(void)
{
    u8 sta;
    sta=((SCB->CCR)>>17)&0X01;
    return sta;
}

//�ж�I_Dache�Ƿ��
//����ֵ:0 �رգ�1 ��
u8 Get_DCahceSta(void)
{
    u8 sta;
    sta=((SCB->CCR)>>16)&0X01;
    return sta;
}

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
//__asm void WFI_SET(void)
//{
//	WFI;
//}
////�ر������ж�(���ǲ�����fault��NMI�ж�)
//__asm void INTX_DISABLE(void)
//{
//	CPSID   I
//	BX      LR
//}
////���������ж�
//__asm void INTX_ENABLE(void)
//{
//	CPSIE   I
//	BX      LR
//}
////����ջ����ַ
////addr:ջ����ַ
//__asm void MSR_MSP(u32 addr)
//{
//	MSR MSP, r0 			//set Main Stack value
//	BX r14
//}
