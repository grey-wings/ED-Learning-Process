/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "lcd.h"
#include "i2c.h"
#include "ds18b20.h"
#include "seg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define print(X) HAL_UART_Transmit(&huart1,(uint8_t *)X,strlen(X),1000)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t adcValue[2];
char show[45];
uint8_t segState,aoState,lcdState,preaoStare,preTempLimit=30;
// segState表示当前数码管的显示状态，0表示温度超限参数，1表示比较通道参数
// aoState表示当前比较通道，0表示AO1，1表示AO2
// lcdState表示当前界面，0表示Main，1表示Para
uint32_t IC2ReadValue1,IC2ReadValue2,compare;
uint32_t CaptureNumber;
float t;
uint8_t tempLimit=30,nowElec=1,pwmflag,nowPara,lcdflag,segflag;
// nowElec若为1则表示当前正在捕获上升沿
// nowPara代表当前选中的参数，0表示选中温度阈值，1表示选中输出通道
uint16_t n,time;
uint8_t rxbuff,pos,usartflag,ledflag;
char ch[30];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void x24c02_write(uint8_t address,uint8_t data)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(address);
	I2CWaitAck();
	I2CSendByte(data);
	I2CWaitAck();
	I2CStop();
}
uint8_t x24c02_read(uint8_t address)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(address);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	uint8_t val=I2CReceiveByte();
	I2CSendNotAck();
	I2CStop();
	return val;
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ledProc()
{
	if (ledflag && t>tempLimit && compare/100.0>adcValue[aoState]/4096.0)
		GPIOC->BSRR=0x81007E00;
	else if (compare/100.0>adcValue[aoState]/4096.0)
		GPIOC->BSRR=0x0100FE00;
	else if (ledflag && t>tempLimit)
		GPIOC->BSRR=0x80007F00;
	else
		GPIOC->BSRR=0x0000FF00;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
}
void lcdProc()
{
	if (!lcdState)
	{
		LCD_DisplayStringLine(Line0,(u8 *)"        Main        ");
		sprintf(show,"AO1:%.2fV           ",adcValue[0]/4096.0*3.3);
		LCD_DisplayStringLine(Line2,(u8 *)show);
		sprintf(show,"AO2:%.2fV           ",adcValue[1]/4096.0*3.3);
		LCD_DisplayStringLine(Line3,(u8 *)show);
		sprintf(show,"PWM2:%u%%             ",compare);
		LCD_DisplayStringLine(Line4,(u8 *)show);
		sprintf(show,"Temp:%.2fC          ",t);
		LCD_DisplayStringLine(Line5,(u8 *)show);
		//sprintf(show,"1:%u\r",TIM7->CNT);
		//print(show);
		//sprintf(show,"2:%u\r",TIM7->CNT);
		//print(show);
		sprintf(show,"N:%u                 ",n);
		LCD_DisplayStringLine(Line6,(u8 *)show);
	}
	else
	{
		LCD_DisplayStringLine(Line0,(u8 *)"        Para        ");
		if (nowPara)
		{
			LCD_SetBackColor(Yellow);
			sprintf(show,"X:AO%u               ",aoState+1);
			LCD_DisplayStringLine(Line4,(u8 *)show);
			LCD_SetBackColor(Black);
			sprintf(show,"T:%u                 ",tempLimit);
			LCD_DisplayStringLine(Line2,(u8 *)show);
		}
		else
		{
			LCD_SetBackColor(Yellow);
			sprintf(show,"T:%u                 ",tempLimit);
			LCD_DisplayStringLine(Line2,(u8 *)show);
			LCD_SetBackColor(Black);
			sprintf(show,"X:AO%u               ",aoState+1);
			LCD_DisplayStringLine(Line4,(u8 *)show);
		}
		LCD_DisplayStringLine(Line3,(u8 *)"                    ");
		LCD_DisplayStringLine(Line5,(u8 *)"                    ");
		LCD_DisplayStringLine(Line6,(u8 *)"                    ");
	}
}
void keyscan()
{
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET);
		if (lcdState)
		{
			if (preaoStare!=aoState || preTempLimit!=tempLimit)
			{
				n++;
				x24c02_write(0x46,n);
			}
		}
		else
		{
			preaoStare=aoState;
			preTempLimit=tempLimit;
		}
		lcdState=!lcdState;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET);
		nowPara=!nowPara;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET && time<80)
		{
			HAL_Delay(10);
			time++;
		}
		LCD_SetBackColor(Yellow);
		if (nowPara)
		{
			aoState=!aoState;
			sprintf(show,"X:AO%u               ",aoState+1);
			LCD_DisplayStringLine(Line4,(u8 *)show);
		}
		else
		{
			tempLimit++;
			if (tempLimit==41)
				tempLimit=20;
			sprintf(show,"T:%u                 ",tempLimit);
			LCD_DisplayStringLine(Line2,(u8 *)show);
		}
		if (nowPara==0 && time>=80 && HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
		{
			while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
			{
				HAL_Delay(100);
				tempLimit++;
				if (tempLimit==41)
					tempLimit=20;
				sprintf(show,"T:%u                 ",tempLimit);
				LCD_DisplayStringLine(Line2,(u8 *)show);
			}
		}
		LCD_SetBackColor(Black);
		time=0;	
	}
	if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET && time<80)
		{
			HAL_Delay(10);
			time++;
		}
		LCD_SetBackColor(Yellow);
		if (nowPara)
		{
			aoState=!aoState;
			sprintf(show,"X:AO%u               ",aoState+1);
			LCD_DisplayStringLine(Line4,(u8 *)show);
		}
		else
		{
			tempLimit--;
			if (tempLimit==19)
				tempLimit=40;
			sprintf(show,"T:%u                 ",tempLimit);
			LCD_DisplayStringLine(Line2,(u8 *)show);
		}
		if (nowPara==0 && time>=80 && HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
		{
			while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
			{
				HAL_Delay(100);
				tempLimit--;
				if (tempLimit==19)
					tempLimit=40;
				sprintf(show,"T:%u                 ",tempLimit);
				LCD_DisplayStringLine(Line2,(u8 *)show);
			}
		}
		LCD_SetBackColor(Black);
		time=0;	
	}
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
  MX_TIM3_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_DisplayStringLine(Line1,(u8 *)"                    ");
	LCD_DisplayStringLine(Line7,(u8 *)"                    ");
	LCD_DisplayStringLine(Line8,(u8 *)"                    ");
	LCD_DisplayStringLine(Line9,(u8 *)"                    ");
	GPIOC->BSRR=0x0000FF00;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_TIM_Base_Start(&htim6);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim4);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_ADCEx_Calibration_Start(&hadc2,ADC_SINGLE_ENDED);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t *)adcValue,2);
	HAL_TIM_IC_Start_IT(&htim3,TIM_CHANNEL_2);
	HAL_UART_Receive_IT(&huart1,&rxbuff,1);
	if (x24c02_read(0x18)==0x30 && x24c02_read(0x28)==0x36 && x24c02_read(0x38)==0x69)
	{
		n=x24c02_read(0x46);
	}
	else
	{
		x24c02_write(0x46,0);
		x24c02_write(0x18,0x40);
		x24c02_write(0x28,0x36);
		x24c02_write(0x38,0x69);
//		HAL_Delay(500);
//		int x=x24c02_read(0x18);
//		HAL_Delay(500);
//		int y=x24c02_read(0x28);
//		HAL_Delay(500);
//		int z=x24c02_read(0x38);
//		sprintf(show,"%X\r%X\r%X\r",x,y,z);
//		print(show);
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if (segflag)
		{
			segflag=0;
			segState=!segState;
			if (segState)
			{
				if (aoState)
					seg_show(10,0,2);
				else
					seg_show(10,0,1);
			}
			else
			{
				seg_show(11,tempLimit/10,tempLimit%10);
			}
		}
		if (CaptureNumber==3)
		{
			CaptureNumber=0;
			compare=IC2ReadValue1*100/IC2ReadValue2;
		}
		if (lcdflag)
		{
			t=ds18b20_read()/16.0;
			lcdflag=0;
			if (compare/100.0>adcValue[aoState]/4096.0)
			{
				
				sprintf(show,"$%.2f\r\n",t);
				print(show);
			}
		}
		keyscan();
		lcdProc();
		if (usartflag)
		{
			usartflag=0;
			if (ch[0]=='S' && ch[1]=='T' && ch[2]=='\r' && ch[3]=='\n')
			{
				sprintf(show,"$%.2f\r\n",t);
				print(show);
			}
			else if (ch[0]=='P' && ch[1]=='A' && ch[2]=='R' && ch[3]=='A' && ch[4]=='\r' && ch[5]=='\n')
			{
				sprintf(show,"#%u,AO%u\r\n",tempLimit,aoState+1);
				print(show);
			}
			strcpy(ch,"");
		}
		ledProc();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance==TIM7)
	{
		segflag=1;
	}
	if (htim->Instance==TIM2)
	{
		lcdflag=1;
	}
	if (htim->Instance==TIM4)
	{
		ledflag=!ledflag;
	}
}
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
  if (CaptureNumber==0)
	{
		CaptureNumber++;
		__HAL_TIM_SetCounter(&htim3,0);
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_FALLING);
	}
	else if (CaptureNumber==1)
	{
		CaptureNumber++;
		IC2ReadValue1=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2);
		__HAL_TIM_SET_CAPTUREPOLARITY(&htim3,TIM_CHANNEL_2,TIM_INPUTCHANNELPOLARITY_RISING);
	}
	else if (CaptureNumber==2)
	{
		CaptureNumber++;
		IC2ReadValue2=HAL_TIM_ReadCapturedValue(&htim3,TIM_CHANNEL_2);
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  ch[pos++]=rxbuff;
	if (rxbuff=='\n')
	{
		usartflag=1;
		ch[pos]='\0';
		pos=0;
	}
	HAL_UART_Receive_IT(&huart1,&rxbuff,1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
