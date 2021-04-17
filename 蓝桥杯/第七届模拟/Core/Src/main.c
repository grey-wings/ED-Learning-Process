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
#include "lcd.h"
#include "i2c.h"
#include "string.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
RTC_DateTypeDef userDate1,userDate2;
RTC_TimeTypeDef userTime1,userTime2;
typedef struct
{
	uint16_t hour,minu,seco;
}UploadT;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define print(X) HAL_UART_Transmit(&huart1,(uint8_t *)X,strlen(X),1000);
int fgetc(FILE *f)
{
	uint8_t ch;
	HAL_UART_Receive(&huart1,&ch,1,0xFFFF);
	return ch;
}
uint8_t x24c02_read(uint8_t address)
{
	unsigned char val;
	
	I2CStart(); 
	I2CSendByte(0xa0);
	I2CWaitAck(); 
	
	I2CSendByte(address);
	I2CWaitAck(); 
	
	I2CStart();
	I2CSendByte(0xa1); 
	I2CWaitAck();
	val = I2CReceiveByte(); 
	I2CWaitAck();
	I2CStop();
	
	return(val);
}

void x24c02_write(unsigned char address,unsigned char info)
{
	I2CStart(); 
	I2CSendByte(0xa0); 
	I2CWaitAck(); 
	
	I2CSendByte(address);	
	I2CWaitAck(); 
	I2CSendByte(info); 
	I2CWaitAck(); 
	I2CStop();
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float volt,k=0.1;
char showVolt[22],showk[22],showTime[22],order[20],upload[30];
uint16_t ADC_Value[20];
uint32_t value;
uint8_t lcdState=1,ledEnable=1,GUIstate=0;  // GUIstate=0时是1界面，等于1时是2界面（设置界面）
uint8_t rxbuff,orderpoint;
UploadT ut,nt;      // upload time, now time
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void keyscan()
{
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_0)==GPIO_PIN_RESET);
		ledEnable=!ledEnable;
	}
	if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET)
	{
		while (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_1)==GPIO_PIN_RESET);
		GUIstate=!GUIstate;
		if (GUIstate)
		{
			LCD_DisplayStringLine(Line2,(u8 *)"                    ");
			LCD_DisplayStringLine(Line1,(u8 *)"      Setting       ");
			LCD_DisplayStringLine(Line3,(u8 *)"                    ");
			LCD_DisplayStringLine(Line4,(u8 *)"                    ");
			LCD_DisplayStringLine(Line5,(u8 *)"                    ");
			LCD_DisplayStringLine(Line6,(u8 *)"                    ");
			LCD_DisplayStringLine(Line7,(u8 *)"                    ");
			LCD_DisplayStringLine(Line8,(u8 *)"                    ");
			LCD_DisplayStringLine(Line9,(u8 *)"                   2");
		}
		else
		{
			sprintf(showk,"    k:%.1f           ",k);
			LCD_DisplayStringLine(Line3,(u8 *)showk);
		}
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
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_RTC_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	if (x24c02_read(0x01)!=99 && x24c02_read(0x25)!=88)
	{
		x24c02_write(0x01,99);
		x24c02_write(0x25,88);
		x24c02_write(0x08,1);
	}
	HAL_TIM_Base_Start_IT(&htim6);       // flush LCD
	//HAL_TIM_Base_Start_IT(&htim7);       // flush LED
	HAL_TIM_Base_Start(&htim3);
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
	GPIOC->BSRR=0x0000FF00;
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
	LCD_Init();
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);
	LCD_DisplayStringLine(Line0,(u8 *)"                    ");
	LCD_DisplayStringLine(Line2,(u8 *)"                    ");
	sprintf(showk,"    k:%.1f           ",k);
	LCD_DisplayStringLine(Line3,(u8 *)showk);
	LCD_DisplayStringLine(Line4,(u8 *)"                    ");
	LCD_DisplayStringLine(Line6,(u8 *)"                    ");
	LCD_DisplayStringLine(Line8,(u8 *)"                    ");
	LCD_DisplayStringLine(Line9,(u8 *)"                   1");
	HAL_ADC_Start_DMA(&hadc2,(uint32_t *)ADC_Value,20);
	HAL_UART_Receive_IT(&huart1,&rxbuff,1);
	ut.hour=0;ut.minu=0;ut.seco=0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		keyscan();
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
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	value=0;
  for (int i=0;i<20;i++)
	{
		value+=ADC_Value[i];
	}
	if (!GUIstate)
	{
		volt=(float)value/4096.0*3.3/20;
		sprintf(showVolt,"    V:%.2fV         ",volt);
		LCD_DisplayStringLine(Line1,(u8 *)showVolt);
		if ((volt>k*3.3)&&ledEnable)
		{
			LCD_DisplayStringLine(Line5,(u8 *)"    LED:ON          ");
			HAL_TIM_Base_Start_IT(&htim7);
		}
		else
		{
			LCD_DisplayStringLine(Line5,(u8 *)"    LED:OFF         ");
			GPIOC->BSRR=0x0000FF00;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
			HAL_TIM_Base_Stop_IT(&htim7);
		}
	}
	
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance==TIM6)
	{
		if (!GUIstate)
		{
			HAL_RTC_GetTime(&hrtc,&userTime1,RTC_FORMAT_BCD);
			HAL_RTC_GetDate(&hrtc,&userDate1,RTC_FORMAT_BCD);
			HAL_RTC_GetTime(&hrtc,&userTime2,RTC_FORMAT_BCD);
			HAL_RTC_GetDate(&hrtc,&userDate2,RTC_FORMAT_BCD);
			if (!(userTime1.Hours==userTime2.Hours && userTime1.Minutes==userTime2.Minutes
				  && userTime1.Seconds==userTime2.Seconds))
			{
				HAL_RTC_GetTime(&hrtc,&userTime1,RTC_FORMAT_BCD);
				HAL_RTC_GetDate(&hrtc,&userDate1,RTC_FORMAT_BCD);
			}
			nt.hour=(userTime1.Hours >> 4)*10+(userTime1.Hours & 15)%10;
			nt.minu=(userTime1.Minutes >> 4)*10+(userTime1.Minutes & 15)%10;
			nt.seco=(userTime1.Seconds >> 4)*10+(userTime1.Seconds & 15)%10;
			sprintf(showTime,"    T:%02u-%02u-%02u         ",nt.hour,nt.minu,nt.seco);
			if (nt.hour==ut.hour && nt.minu==ut.minu && nt.seco==ut.seco)
			{
				sprintf(upload,"%.2f+%.1f+%02u%02u%02u\n\r",volt,k,ut.hour,ut.minu,ut.seco);
				print(upload)
			}
			//sprintf(showTime,"%d\n\r",userTime1.Seconds);
			LCD_DisplayStringLine(Line7,(u8 *)showTime);
		}
	}
	 if (htim->Instance==TIM7)
	{
		if (lcdState)
		{
			GPIOC->BSRR=0x0100FE00;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		else
		{
			GPIOC->BSRR=0x0000FF00;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_2,GPIO_PIN_RESET);
		}
		lcdState=!lcdState;
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (rxbuff!='\n')
	{
		order[orderpoint++]=rxbuff;
	}
	else
	{
		order[orderpoint]='\0';
		if (order[0]=='k')
		{
			k=(float)(order[3]-48)/10.0;
			x24c02_write(0x08,order[3]-48);
			sprintf(showk,"    k:%.1f           ",k);
			LCD_DisplayStringLine(Line3,(u8 *)showk);
			print("ok\n\r")
		}
		orderpoint=0;
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
