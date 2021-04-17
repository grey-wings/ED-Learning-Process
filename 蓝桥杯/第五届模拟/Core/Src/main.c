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
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "lcd.h"
#include "string.h"
#include "i2c.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
RTC_TimeTypeDef rtc_time,previousTime,orderTime;
RTC_DateTypeDef rtc_date;
int adc_flag;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define print(X) HAL_UART_Transmit(&huart1,(uint8_t *)X,strlen(X),1000);
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
char orders[30];
char showPA1[22],showPA2[22],showTime[22],showChan[22],showCom[22],order[30],ttry[30];
uint8_t compare1=80,compare2=80,pa1State=1,pa2State=1,rxbuff,point,orderOut;// 1->PWM,0->lo/w
// orderOut表示当前输出的通道（1或2）,若为0，则表示没有命令。
uint8_t isHigh,continueTime; // 表示命令是否要求当前通道输出PWM波
uint32_t adc_value;

  char s[30];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void keyscan()
{
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET);
		if (pa1State)
		{
			TIM2->CCR2=0;
		}
		else
		{
			TIM2->CCR2=compare1;
		}
		pa1State=!pa1State;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET);
		compare1+=10;
		if (compare1==110)
			compare1=0;
		TIM2->CCR2=compare1;
		sprintf(showPA1,"  PWM-PA1:%u%%       ",compare1);
		LCD_DisplayStringLine(Line0,(u8 *)showPA1);
		x24c02_write(0x01,compare1);
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2)==GPIO_PIN_RESET);
		if (pa2State)
		{
			TIM2->CCR3=0;
		}
		else
		{
			TIM2->CCR3=compare1;
		}
		pa2State=!pa2State;
	}
	if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET);
		compare2+=10;
		if (compare2==110)
			compare2=0;
		TIM2->CCR3=compare2;
		sprintf(showPA2,"  PWM-PA2:%u%%       ",compare2);
		LCD_DisplayStringLine(Line2,(u8 *)showPA2);
		x24c02_write(0x02,compare2);
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
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */
	LCD_Init();
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	sprintf(showPA1,"  PWM-PA1:%u%%       ",compare1);
	sprintf(showPA2,"  PWM-PA2:%u%%       ",compare2);
	LCD_DisplayStringLine(Line0,(u8 *)showPA1);
	LCD_DisplayStringLine(Line1,(u8 *)"                    ");
	LCD_DisplayStringLine(Line2,(u8 *)showPA2);
	LCD_DisplayStringLine(Line3,(u8 *)"                    ");
	HAL_RTC_GetTime(&hrtc,&rtc_time,RTC_FORMAT_BIN);
	HAL_RTC_GetDate(&hrtc,&rtc_date,RTC_FORMAT_BIN);
	sprintf(showTime,"  Time:%02u:%02u:%02u     ",rtc_time.Hours,rtc_time.Minutes,rtc_time.Seconds);
	previousTime=rtc_time;
	LCD_DisplayStringLine(Line4,(u8 *)showTime);
	LCD_DisplayStringLine(Line5,(u8 *)"                    ");
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
	TIM2->CCR3=compare2;
	LCD_DisplayStringLine(Line6,(u8 *)"  Channel:PA2       ");
	LCD_DisplayStringLine(Line7,(u8 *)"                    ");
	LCD_DisplayStringLine(Line8,(u8 *)"  Command:          ");
	LCD_DisplayStringLine(Line9,(u8 *)"        None        ");
	GPIOC->BSRR=0x0200FD00;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_UART_Receive_IT(&huart1,&rxbuff,1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		keyscan();
			//LCD_DisplayStringLine(Line7,(u8 *)s);
		HAL_RTC_GetTime(&hrtc,&rtc_time,RTC_FORMAT_BIN);
		HAL_RTC_GetDate(&hrtc,&rtc_date,RTC_FORMAT_BIN);
		if (previousTime.Seconds!=rtc_time.Seconds)
		{	
			sprintf(showTime,"  Time:%02u:%02u:%02u     ",rtc_time.Hours,rtc_time.Minutes,rtc_time.Seconds);
			previousTime=rtc_time;
			LCD_DisplayStringLine(Line4,(u8 *)showTime);
		}
		if (orderOut && rtc_time.Hours==orderTime.Hours && rtc_time.Minutes==orderTime.Minutes
			  && rtc_time.Seconds==orderTime.Seconds)
		{
			if (orderOut==1)
			{
				if (isHigh)
				{
					HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
					TIM2->CCR2=compare1;
					LCD_DisplayStringLine(Line6,(u8 *)"  Channel:PA1       ");
					GPIOC->BSRR=0x0100FE00;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
					isHigh=0;
				}
				else
				{
					TIM2->CCR2=0;
					orderOut=0;
				}
			}
			if (orderOut==2)
			{
				if (isHigh)
				{
					HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
					TIM2->CCR3=compare2;
					LCD_DisplayStringLine(Line6,(u8 *)"  Channel:PA2       ");
					GPIOC->BSRR=0x0200FD00;
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
					isHigh=0;
				}
				else
				{
					TIM2->CCR3=0;
					orderOut=0;
				}
			}
			orderTime.Seconds+=continueTime;
			if (orderTime.Seconds>=60)
				{
					orderTime.Seconds-=60;
					orderTime.Minutes++;
					if (orderTime.Minutes>=60)
					{
						orderTime.Minutes-=60;
						orderTime.Hours++;
					}
					if (orderTime.Hours==24)
					{
						orderTime.Hours=0;
					}
				}
			continueTime=0;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC|RCC_PERIPHCLK_USART1
                              |RCC_PERIPHCLK_ADC12;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Adc12ClockSelection = RCC_ADC12CLKSOURCE_SYSCLK;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (rxbuff!='S')
	{
		order[point++]=rxbuff;
	}
	else
	{
		order[point++]='\0';
		orderTime.Hours=(order[0]-'0')*10+order[1]-'0';
		orderTime.Minutes=(order[3]-'0')*10+order[4]-'0';
		orderTime.Seconds=(order[6]-'0')*10+order[7]-'0';
		if (order[11]=='1')
		{
			orderOut=1;
		}
		else
		{
			orderOut=2;
		}
		continueTime=order[13]-'0';
		isHigh=1;
		point=0;
		LCD_DisplayStringLine(Line9,(u8 *)order);
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
