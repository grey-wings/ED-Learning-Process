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
#include "rtc.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "lcd.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	UP=0,
	DOWN=1
}Direction;
typedef enum
{
	COMMAND=0,
	RUN=1
}whileState;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
RTC_DateTypeDef sDates;
RTC_TimeTypeDef sTimes;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char nowFlatChar[22],nowTime[22];
uint8_t nowflat=1,twinkle=1;
uint8_t lightNow=5;                   // 用于存放流水灯当前亮的那个
uint8_t lightState[5]={0,0,0,0,0};    // 用于存放LD1到LD4的状态
Direction direction=DOWN;
whileState ws=COMMAND;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void keyscan()
{
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET);
		if (nowflat!=1)
		{
			lightState[1]=1;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		TIM2->CNT=0;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET);
		if (nowflat!=2)
		{
			lightState[2]=1;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		TIM2->CNT=0;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET);
		if (nowflat!=3)
		{
			lightState[3]=1;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		TIM2->CNT=0;
	}
	if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET);
		if (nowflat!=4)
		{
			lightState[4]=1;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		TIM2->CNT=0;
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
  MX_RTC_Init();
  MX_TIM3_Init();
  MX_TIM7_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
	
	LCD_Init();
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_DisplayStringLine(Line0,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line1,(uint8_t *)"      Now Flat      ");
	sprintf(nowFlatChar,"          %d         ",nowflat);
	LCD_DisplayStringLine(Line3,(uint8_t *)nowFlatChar);
	LCD_DisplayStringLine(Line2,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line4,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line5,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line6,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line7,(uint8_t *)"                    ");
	LCD_DisplayStringLine(Line8,(uint8_t *)"                    ");
	GPIOC->BSRR=0x0000FF00;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_TIM_Base_Start_IT(&htim7);
	HAL_TIM_Base_Start(&htim3);            
	
			HAL_TIM_Base_Start_IT(&htim2);        // 用于检测按键是否有1s未按下
	//HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if (ws==COMMAND)
		{
			keyscan();
		}
		else
		{
			for (int j=nowflat+1;j<=4;j++)
			{
				direction=UP;
				if (lightState[j])
				{
					// close the door
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
					TIM3->PSC=799;
					TIM3->ARR=99;
					TIM3->CCR1=50;
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
					HAL_Delay(4000);
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
					// go up
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET);
					TIM3->PSC=399;
					TIM3->ARR=99;
					TIM3->CCR2=80;
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
					HAL_TIM_Base_Start_IT(&htim6);
					HAL_Delay(6000);
					lightState[j]=0;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
					if (j==2) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
					if (j==3) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
					if (j==4) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_SET);
					HAL_TIM_Base_Stop_IT(&htim6);
					GPIOC->BSRR=0x0000F000;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
					nowflat=j;
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
					// open the door
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
					TIM3->PSC=799;
					TIM3->ARR=99;
					TIM3->CCR1=60;
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
					HAL_Delay(4000);
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				}
			}
			// 下行
			for (int j=nowflat-1;j>=1;j--)
			{ 
				direction=DOWN;
				if (lightState[j])
				{
					// close the door
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
					TIM3->PSC=799;
					TIM3->ARR=99;
					TIM3->CCR1=50;
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
					HAL_Delay(4000);
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
					// go up
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET);
					TIM3->PSC=399;
					TIM3->ARR=99;
					TIM3->CCR2=60;
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
					HAL_TIM_Base_Start_IT(&htim6);
					HAL_Delay(6000);
					lightState[j]=0;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
					if (j==2) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_SET);
					if (j==3) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_SET);
					if (j==1) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_SET);
					HAL_TIM_Base_Stop_IT(&htim6);
					GPIOC->BSRR=0x0000F000;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
					nowflat=j;
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_2);
					// open the door
					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);
					TIM3->PSC=799;
					TIM3->ARR=99;
					TIM3->CCR1=60;
					HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
					HAL_Delay(4000);
					HAL_TIM_PWM_Stop(&htim3,TIM_CHANNEL_1);
				}
			}
			ws=COMMAND;
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance==TIM2)
	{
		if (ws==COMMAND)
			ws=RUN;
	}
  if (htim->Instance==TIM7)
	{
		HAL_RTC_GetTime(&hrtc,&sTimes,RTC_FORMAT_BCD);
		HAL_RTC_GetDate(&hrtc,&sDates,RTC_FORMAT_BCD);
		sprintf(nowTime,"      %u%u:%u%u:%u%u      ",sTimes.Hours >> 4,sTimes.Hours & 15,
		sTimes.Minutes >> 4,sTimes.Minutes & 15,sTimes.Seconds >> 4,sTimes.Seconds & 15);
		LCD_DisplayStringLine(Line9,(uint8_t *)nowTime);
		if (twinkle)
		{
			sprintf(nowFlatChar,"          %d         ",nowflat);
			LCD_DisplayStringLine(Line3,(uint8_t *)nowFlatChar);
		}
		else
		{
			LCD_DisplayStringLine(Line3,(uint8_t *)"                    ");
		}
//		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
//		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		GPIOC->BSRR=0x0000FF00;
		if (lightState[1]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		if (lightState[2]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
		if (lightState[3]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
		if (lightState[4]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
		twinkle=!twinkle;
	}
	if (htim->Instance==TIM6)
	{
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
		switch (lightNow)
		{
			case 5:
			{
				GPIOC->BSRR=0x1000EF00;
				break;
			}
			case 6:
			{
				GPIOC->BSRR=0x2000DF00;
				break;
			}
			case 7:
			{
				GPIOC->BSRR=0x4000BF00;
				break;
			}
			case 8:
			{
				GPIOC->BSRR=0x80007F00;
				break;
			}
		}
		if (lightState[1]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_8,GPIO_PIN_RESET);
		if (lightState[2]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_9,GPIO_PIN_RESET);
		if (lightState[3]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10,GPIO_PIN_RESET);
		if (lightState[4]) HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		if (direction==UP)
		{
			lightNow++;
			if (lightNow==9) lightNow=5;
		}
		else
		{
			lightNow--;
			if (lightNow==4) lightNow=8;
		}
	}
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
