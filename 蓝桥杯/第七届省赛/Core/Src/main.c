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
#include "i2c.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define print(X) HAL_UART_Transmit(&huart1,(uint8_t *)X,strlen(X),1000);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void x24c02_write(u8 address,u8 data)
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
u8 x24c02_read(u8 address)
{
	I2CStart();
	I2CSendByte(0xa0);
	I2CWaitAck();
	I2CSendByte(address);
	I2CWaitAck();
	I2CStart();
	I2CSendByte(0xa1);
	I2CWaitAck();
	u8 val=I2CReceiveByte();
	I2CStop();
	return val;
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char ttry[30],showADC[22],showLevel[22],showHeight[22],setlow[22],setmid[22],sethigh[22],outt[20];
float adc_value,height;
uint16_t ADC_Value[1],level,adc_flag=0,low=30,high=70,mid=50,previousk;
uint8_t issetting=0,rxbuff; // 判断当前是否是设置状态
uint8_t selectState=1;   // 判断当前要调整的参数，1：low；2：mid；3：high
uint16_t times2,times3;   // 判断令LD2，LD3闪烁的中断执行了几次
uint16_t light1,light2,light3; // 判断要闪的灯的状态
uint16_t ld2_flag,ld3_flag; // 判断当前灯是否要闪
uint16_t first=1;  // 判断是否第一次过主循环
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void keyscan()
{
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET);
		if (issetting)
		{
			LCD_DisplayStringLine(Line0,(uint8_t *)"                    ");
			LCD_DisplayStringLine(Line1,(uint8_t *)"  Liquid Level      ");
			LCD_DisplayStringLine(Line2,(uint8_t *)"                    ");
			LCD_DisplayStringLine(Line4,(uint8_t *)"                    ");
			LCD_DisplayStringLine(Line6,(uint8_t *)"                    ");
			LCD_DisplayStringLine(Line8,(uint8_t *)"                    ");
			LCD_DisplayStringLine(Line9,(uint8_t *)"                    ");
			sprintf(showHeight,"  Height:%dcm        ",(int)height);
			LCD_DisplayStringLine(Line3,(uint8_t *)showHeight);
			sprintf(showADC,"  ADC:%.2fV         ",adc_value);
			LCD_DisplayStringLine(Line5,(uint8_t *)showADC);
			sprintf(showLevel,"  Level:%u            ",level);
			LCD_DisplayStringLine(Line7,(uint8_t *)showLevel);
		}
		else
		{
			LCD_DisplayStringLine(Line1,(uint8_t *)"  Parameter Setup   ");
			LCD_SetTextColor(Red);
			sprintf(setlow,"  Threshold1:%ucm     ",low);
			LCD_DisplayStringLine(Line3,(uint8_t *)setlow);
			LCD_SetTextColor(White);
			sprintf(setmid,"  Threshold2:%ucm     ",mid);
			LCD_DisplayStringLine(Line5,(uint8_t *)setmid);
			sprintf(sethigh,"  Threshold3:%ucm     ",high);
			LCD_DisplayStringLine(Line7,(uint8_t *)sethigh);
		}
		issetting=!issetting;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET);
		if (issetting)
		{
			selectState++;
		if (selectState==4)
			selectState=1;
		switch (selectState)
		{
			case 1:
			{
				LCD_SetTextColor(Red);
				sprintf(setlow,"  Threshold1:%ucm     ",low);
				LCD_DisplayStringLine(Line3,(uint8_t *)setlow);
				LCD_SetTextColor(White);
				sprintf(sethigh,"  Threshold3:%ucm     ",high);
				LCD_DisplayStringLine(Line7,(uint8_t *)sethigh);
				break;
			}
			case 2:
			{
				LCD_SetTextColor(Red);
				sprintf(setmid,"  Threshold2:%ucm     ",mid);
				LCD_DisplayStringLine(Line5,(uint8_t *)setmid);
				LCD_SetTextColor(White);
				sprintf(setlow,"  Threshold1:%ucm     ",low);
				LCD_DisplayStringLine(Line3,(uint8_t *)setlow);
				break;
			}
			case 3:
			{
				LCD_SetTextColor(Red);
				sprintf(sethigh,"  Threshold3:%ucm     ",high);
				LCD_DisplayStringLine(Line7,(uint8_t *)sethigh);
				LCD_SetTextColor(White);
				sprintf(setmid,"  Threshold2:%ucm     ",mid);
				LCD_DisplayStringLine(Line5,(uint8_t *)setmid);
				break;
			}
		}
		}
		
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET);
		if (issetting)
		{
			switch (selectState)
			{
				case 1:
				{
					if (low<=90)
						low+=5;
					x24c02_write(0x88,low);
					LCD_SetTextColor(Red);
					sprintf(setlow,"  Threshold1:%ucm     ",low);
					LCD_DisplayStringLine(Line3,(uint8_t *)setlow);
					LCD_SetTextColor(White);
					break;
				}
			  case 2:
				{
					if (mid<=90)
						mid+=5;
					x24c02_write(0x89,mid);
					LCD_SetTextColor(Red);
					sprintf(setmid,"  Threshold2:%ucm     ",mid);
					LCD_DisplayStringLine(Line5,(uint8_t *)setmid);
					LCD_SetTextColor(White);
					break;
				}
				case 3:
				{
					if (high<=90)
						high+=5;
					x24c02_write(0x90,high);
					LCD_SetTextColor(Red);
					sprintf(sethigh,"  Threshold3:%ucm     ",high);
					LCD_DisplayStringLine(Line7,(uint8_t *)sethigh);
					LCD_SetTextColor(White);
					break;
				}
			}
		}
	}
	if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET);
		if (issetting)
		{
			switch (selectState)
			{
				case 1:
				{
					if (low>=10)
						low-=5;
					x24c02_write(0x88,low);
					LCD_SetTextColor(Red);
					sprintf(setlow,"  Threshold1:%ucm     ",low);
					LCD_DisplayStringLine(Line3,(uint8_t *)setlow);
					LCD_SetTextColor(White);
					break;
				}
			  case 2:
				{
					if (mid>=10)
						mid-=5;
					x24c02_write(0x89,mid);
					LCD_SetTextColor(Red);
					sprintf(setmid,"  Threshold2:%ucm     ",mid);
					LCD_DisplayStringLine(Line5,(uint8_t *)setmid);
					LCD_SetTextColor(White);
					break;
				}
				case 3:
				{
					if (high>=10)
						high-=5;
					x24c02_write(0x90,high);
					LCD_SetTextColor(Red);
					sprintf(sethigh,"  Threshold3:%ucm     ",high);
					LCD_DisplayStringLine(Line7,(uint8_t *)sethigh);
					LCD_SetTextColor(White);
					break;
				}
			}
		}
	}
}
void exeadc()
{
		adc_value=(float)ADC_Value[0]/4096.0*3.3;
		height=adc_value/3.3*100;
		sprintf(showHeight,"  Height:%dcm        ",(int)height);
		LCD_DisplayStringLine(Line3,(uint8_t *)showHeight);
		sprintf(showADC,"  ADC:%.2fV         ",adc_value);
		LCD_DisplayStringLine(Line5,(uint8_t *)showADC);
		if (height<=low)
		{
			level=0;
		}
		else if (height<=mid)
		{
			level=1;
		}
		else if (height<=high)
		{
			level=2;
		}
		else
		{
			level=3;
		}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_TIM2_Init();
  MX_TIM6_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	HAL_TIM_Base_Start(&htim3);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_ADC_Start_DMA(&hadc2,(uint32_t *)ADC_Value,1);
	LCD_DisplayStringLine(Line1,(uint8_t *)"  Liquid Level      ");
	LCD_DisplayStringLine(Line2,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line4,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line6,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line8,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line9,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line0,(uint8_t *)"                    ");
	previousk=level;
	GPIOC->BSRR=0x0000FF00;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	if (x24c02_read(0x24)!=42 && x24c02_read(0x35)!=53 && x24c02_read(0x67)!=76)
	{
		x24c02_write(0x24,42);
		x24c02_write(0x35,53);
		x24c02_write(0x67,76);
		x24c02_write(0x88,low);
		x24c02_write(0x89,mid);
		x24c02_write(0x90,high);
	}
	HAL_UART_Receive_DMA(&huart1,&rxbuff,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		keyscan();
		if ((adc_flag)&&(!issetting))
		{
			exeadc();
			if ((previousk!=level)&&(!first))
			{
				ld2_flag=1;
				if (level>previousk)
				{
					sprintf(outt,"A:H%u+L%u+U\r\n",(uint16_t)height,level);
					print(outt)
				}
				else
				{
					sprintf(outt,"A:H%u+L%u+D\r\n",(uint16_t)height,level);
					print(outt)
				}
			}
			if (first)
				first=0;
			previousk=level;
			sprintf(showLevel,"  Level:%u            ",level);
			LCD_DisplayStringLine(Line7,(uint8_t *)showLevel);
			adc_flag=0;
		}
		if (ld2_flag && ld3_flag)
		{
			if (light1)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	
			if (light2)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);	
			if (light3)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);	
			GPIOC->BSRR=0x0000F800;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);	
			if (times2==10)
			{
				ld2_flag=0;
				times2=0;
			}
			if (times3==10)
			{
				ld3_flag=0;
				times3=0;
			}
		}
		else if (ld2_flag)
		{
			if (light1)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);	
			if (light2)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
			GPIOC->BSRR=0x0000FC00;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);		
			if (times2==10)
			{
				ld2_flag=0;
				times2=0;
			}			
		}
		else if (ld3_flag)
		{
			if (light1)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
			if (light3)
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);	
			else
				HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);	
			GPIOC->BSRR=0x0000FA00;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);	
			if (times3==10)
			{
				ld3_flag=0;
				times3=0;
			}
		}
		else
		{
			if (light1)
				GPIOC->BSRR=0x0100FE00;
			else
				GPIOC->BSRR=0x0000FF00;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	adc_flag=1;
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance==TIM6)
	{
		light1=!light1;
	}
	if (htim->Instance==TIM2)
	{
		if (ld2_flag)
		{
			light2=!light2;
			times2++;
		}
		if (ld3_flag)
		{
			light3=!light3;
			times3++;
		}
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	ld3_flag=1;
  if (rxbuff=='C')
	{
		sprintf(outt,"C:H%u+L%u\r\n",(uint16_t)height,level);
		print(outt)
	}
	if (rxbuff=='S')
	{
		sprintf(outt,"TL%u+TM%u+TH%u\r\n",low,mid,high);
		print(outt)
	}
	HAL_UART_Receive_DMA(&huart1,&rxbuff,1);
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
