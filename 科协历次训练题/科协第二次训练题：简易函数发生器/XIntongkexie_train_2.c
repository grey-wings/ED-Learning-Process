/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "dac.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "stm32f4xx_it.h"
#include "stdio.h"
#include "touch.h"
#include "tftlcd.h"
#include "delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char UART_STR[100];
uint32_t graph[200];
int fputc(int ch,FILE* f)
{
    uint8_t temp[1]={ch};
    HAL_UART_Transmit(&huart1,temp,1,2);
    return 0;
}
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.1415926535
float Um=3.0;
uint32_t compare=50;
uint32_t frequency=10;   //频率，单位：千赫
uint32_t cases;
char sb[30];
uint8_t RxByte;
uint8_t RxBuff[100];
uint16_t Rx_Count;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

void PWM_Data(uint32_t *D)
{
    uint32_t positive=compare*2;
    for (uint32_t i=0;i<positive+1;i++)
    {
        D[i]=(uint32_t)((Um)*4095.0/3.3);
    }
    for (uint32_t i=positive+1;i<200;i++)
        D[i]=0;
}
void Show_PWM(void)
{
    LCD_Clear(WHITE);
    POINT_COLOR=RED;
    char strff[21];
    char orders[30]="Frequency:(kHz)";
    LCD_ShowString(0,100,48,16,16,"PWM");
    itoa(frequency,strff,10);
    strcat(orders,strff);
    LCD_ShowString(0,200,320,16,16,orders);
    sprintf(strff,"%.2f",Um);
    strcpy(orders,"Peak value:");
    strcat(orders,strff);
    LCD_ShowString(0,300,320 ,16,16,orders);
    itoa(compare,strff,10);
    strcpy(orders,"Duty value:");
    strcat(orders,strff);
    LCD_ShowString(0,400,208,16,16,orders);
}
void SinWave_Data(uint32_t *D)
{
    Um/=2;
    for(uint32_t i=0;i<200;i++)
    {
        D[i]=(uint32_t)((Um*sin(i*PI/100)+Um+0.2)*4095/3.3);
    }
    Um*=2;
}
void Show_sin(void)
{
    LCD_Clear(WHITE);
    POINT_COLOR=RED;
    char strff[21];
    char orders[30]="Frequency(kHz):";
    LCD_ShowString(0,100,48,16,16,"sin");
    itoa(frequency,strff,10);
    strcat(orders,strff);
    LCD_ShowString(0,200,320,16,16,orders);
    strcpy(orders,"Peak value:");
    sprintf(strff,"%.2f",Um);
    strcat(orders,strff);
    LCD_ShowString(0,300,320,16,16,orders);
}
void Triangle_Data(uint32_t *D)
{
    float d[200];
    float k=Um/(2*compare);
    for (uint32_t i=0;i<2*compare;i++)
    {
        d[i]=i*k;
    }
    k=Um/(200-2*compare);
    for (uint32_t i=2*compare;i<200;i++)
    {

        d[i]=Um-(i-2*compare)*k;
    }
    for (uint32_t i=0;i<200;i++)
        D[i]=(uint32_t)((d[i]+0.2)*4095/3.3);
}
void Show_Triangle(void)
{
    LCD_Clear(WHITE);
    POINT_COLOR=RED;
    char strff[21];
    char orders[30]="Frequency(kHz):";
    LCD_ShowString(0,100,200,16,16,"Triangle");
    itoa(frequency,strff,10);
    strcat(orders,strff);
    LCD_ShowString(0,200,320,16,16,orders);
    sprintf(strff,"%.2f",Um);
    strcpy(orders,"Peak value:");
    strcat(orders,strff);
    LCD_ShowString(0,300,320 ,16,16,orders);
    itoa(compare,strff,10);
    strcpy(orders,"Duty value:");
    strcat(orders,strff);
    LCD_ShowString(0,400,320,16,16,orders);
}
void change_lcd()
{
    switch (cases)
    {
        case 0:
        {
            PWM_Data(graph);
            Show_PWM();
            break;
        }
        case 1:
        {
            SinWave_Data(graph);
            Show_sin();
            break;
        }
        case 2:
        {
            Triangle_Data(graph);
            Show_Triangle();
            break;
        }
    }
}
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void print(char* str)
{
    HAL_UART_Transmit(&huart1,str,strlen(str),1000);
}
void printIntln(int v)
{
    memset(UART_STR,0,sizeof(UART_STR));
    itoa(v,UART_STR,10);
    HAL_UART_Transmit(&huart1,UART_STR,strlen(UART_STR),1000);
    print("\n\r");
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin==GPIO_PIN_0)
    {
        compare+=5;
        change_lcd();
    }
    if (GPIO_Pin==GPIO_PIN_2)
    {
        HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
        compare-=5;
        change_lcd();
    }
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    HAL_UART_Transmit(&huart1,RxBuff,20,1000);
    switch (RxBuff[7])
    {
        case 'u':
        {
            float xx=0.0;
            xx+=RxBuff[14]-'0';
            xx+=(RxBuff[16]-'0')*0.1;
            xx+=(RxBuff[17]-'0')*0.01;
            Um=xx;
            change_lcd();
            break;
        }
        case 'c':
        {//change c into 100.
            uint32_t xx=0;
            if (RxBuff[17]=='.')
            {
                xx=100;
            }
            else
            {
                xx+=(RxBuff[14]-'0')*10;
                xx+=RxBuff[15]-'0';
            }
            compare=xx;
            change_lcd();
            break;
        }
        case 'f':
        {
            uint32_t xx=0;
            if (RxBuff[15]=='.')
            {
                xx=RxBuff[14]-'0';
            }
            else
            {
                xx+=(RxBuff[14]-'0')*10;
                xx+=(RxBuff[15]-'0');
            }
            frequency=xx;
            TIM6->ARR=420/frequency;
            change_lcd();
            break;
        }
    }
    HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void ctp_test(void)
{
    // y:320-480; x:160,320,480;
    LCD_Clear(WHITE);
    LCD_Fill(0,320,160,480,GREEN);
    LCD_Fill(160,320,320,480,YELLOW);
    LCD_Fill(320,320,480,480,GRAYBLUE);
    POINT_COLOR=RED;
    LCD_ShowString(80-24,400-8,48,16,16,"PWM");
    LCD_ShowString(240-24,400-8,48,16,16,"sin");
    LCD_ShowString(400-64,400-8,128,16,16,"triangle");
    //print("Format:\nchange u into 3.20\nchange c into 30.0\nchange f into 10.0\nchange f into 8.00\n");
    while(1)
    {
        tp_dev.scan(0);
        if(tp_dev.x[0]<lcddev.width&&tp_dev.y[0]<lcddev.height)
        {
            if(tp_dev.x[0]<160&&tp_dev.y[0]<480&&tp_dev.y[0]>320)
            {                                 /**DMA 串口接收和发送哪个要设为循环 **/
                print("PWM");
                cases=0;
                LCD_Clear(BLACK);
                HAL_TIM_Base_Start_IT(&htim6);
                TIM6->ARR=420/frequency-1;  //计时器溢出一次数组移动一格
                PWM_Data(graph);           //根据峰峰值和占空比生成波形表
                HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *)&graph,200,DAC_ALIGN_12B_R);
                Show_PWM();
                break;
            }
            if(tp_dev.x[0]>160&&tp_dev.x[0]<320&&tp_dev.y[0]<480&&tp_dev.y[0]>320)
            {
                print("sin");
                cases=1;
                LCD_Clear(BLACK);
                HAL_TIM_Base_Start_IT(&htim6);
//                HAL_UART_Receive(&huart1,(uint8_t *)orders,strlen(orders),1000);
                TIM6->ARR=420/frequency-1;  //计时器溢出一次数组移动一格
                SinWave_Data(graph);           //根据峰峰值和占空比生成波形表
                Show_sin();
                HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *) &graph,200-1,DAC_ALIGN_12B_R);
                break;
            }
            if(tp_dev.x[0]<480&&tp_dev.y[0]<480&&tp_dev.y[0]>320)
            {
                 print("triangle");
                 cases=2;
                 LCD_Clear(BLACK);
                 HAL_TIM_Base_Start_IT(&htim6);
                 TIM6->ARR=420/frequency-1;  //计时器溢出一次数组移动一格
                 Triangle_Data(graph);           //根据峰峰值和占空比生成波形表
                 HAL_DAC_Start_DMA(&hdac,DAC_CHANNEL_1,(uint32_t *) &graph,200,DAC_ALIGN_12B_R);
                 Show_Triangle();
                 break;
            }
        }
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
  MX_DAC_Init();
  MX_USART1_UART_Init();
  MX_FSMC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart1,RxBuff,20);
  delay_init(168);
  LCD_Init();
  tp_dev.init();
  //HAL_DAC_Start(&hdac,DAC_CHANNEL_1);
  //HAL_DAC_Init()不需要调用，因为已经在MX_DAC_Init()里面调用过


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  ctp_test();
  while (1)
  {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
