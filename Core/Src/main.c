/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
#include "fonts.h"
#include "test.h"
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

/* Definitions for userControl */
osThreadId_t userControlHandle;
const osThreadAttr_t userControl_attributes = {
  .name = "userControl",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ballControl */
osThreadId_t ballControlHandle;
const osThreadAttr_t ballControl_attributes = {
  .name = "ballControl",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for opponent */
osThreadId_t opponentHandle;
const osThreadAttr_t opponent_attributes = {
  .name = "opponent",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Draw */
osThreadId_t DrawHandle;
const osThreadAttr_t Draw_attributes = {
  .name = "Draw",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
void userPaddle(void *argument);
void ball(void *argument);
void oponentPaddle(void *argument);
void DrawScreen(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


//uint16_t YScrollValue;
uint16_t playerYPosition = 15;
const uint16_t paddleLength = 20;
const uint16_t paddleWidth = 5;

uint16_t opponentYPosition = 15;
const uint16_t opponentBarLength = 18;
const uint16_t opponentBarWidth = 5;


int16_t ballXPosition;
int16_t ballYPosition;

int16_t ballXVelocity;
int16_t ballYVelocity;

int16_t ballXDirection;
int16_t ballYDirection;



const uint16_t ballRadius = 4;
const uint16_t halfBallRadius = ballRadius/2;

const uint16_t leftEdge = 5;
const uint16_t rightEdge = 123;

uint16_t playerScored = 0;
uint16_t opponentScored = 0;



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
/*
	uint8_t buf[12];
		uint16_t raw;
		char msg[10];

		uint16_t YScrollValue;
		uint16_t playerBarYSize = 18;
		uint16_t playerBarXSize = 5;
		uint16_t playerBarYPosition;
*/
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
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  SSD1306_Init();

  SSD1306_GotoXY(0,0);

  SSD1306_Puts("starting", &Font_11x18, 1);
  SSD1306_GotoXY(10, 30);
  SSD1306_Puts("    WORLD: ", &Font_11x18, 1);
  SSD1306_UpdateScreen();

  HAL_Delay(200);

  SSD1306_Clear();






  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of userControl */
  userControlHandle = osThreadNew(userPaddle, NULL, &userControl_attributes);

  /* creation of ballControl */
  ballControlHandle = osThreadNew(ball, NULL, &ballControl_attributes);

  /* creation of opponent */
  opponentHandle = osThreadNew(oponentPaddle, NULL, &opponent_attributes);

  /* creation of Draw */
  DrawHandle = osThreadNew(DrawScreen, NULL, &Draw_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

/*
	  //SSD1306_Init();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  YScrollValue = HAL_ADC_GetValue(&hadc1);

	  //ScrollValue = 0;
	  //0//4095

	  if(YScrollValue == 0) //Up
	  {

		  if(playerBarYPosition < 255)
		  {
			  playerBarYPosition += 1;
			  SSD1306_Clear();
			   SSD1306_GotoXY(0,0);
			   SSD1306_Puts("up", &Font_11x18, 1);
			   SSD1306_UpdateScreen();

			   //HAL_Delay(2000);


		  }

	  }

	  if(YScrollValue == 4095)//down
	  {
		  if(playerBarYPosition > 0)
		  {
			  playerBarYPosition -= 1;
			  SSD1306_Clear();
			   SSD1306_GotoXY(0,0);
			   SSD1306_Puts("down", &Font_11x18, 1);
			   SSD1306_UpdateScreen();

			   //HAL_Delay(2000);
		  }

	  }






	  sprintf(msg, "%hu\r\n", YScrollValue);
	 // HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	  //SSD1306_GotoXY(0,0);
	  //SSD1306_Puts(&msg, &Font_11x18, 1);
	  //SSD1306_UpdateScreen();



	  HAL_Delay(10);
*/



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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00300F38;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LD3_Pin */
  GPIO_InitStruct.Pin = LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD3_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_userPaddle */
/**
  * @brief  Function implementing the userControl thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_userPaddle */
void userPaddle(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */


	uint16_t YScrollValue;

  for(;;)
  {


	  //SSD1306_Init();
	  HAL_ADC_Start(&hadc1);
	  HAL_ADC_PollForConversion(&hadc1, 10);
	  YScrollValue = HAL_ADC_GetValue(&hadc1);

	  if(YScrollValue == 0) //Up
	  {

		  if(playerYPosition < SSD1306_HEIGHT - paddleLength)
		  {
			  playerYPosition += 1;

		  }

	  }

	  if(YScrollValue == 4095)//down
	  {
		  if(playerYPosition > 0)
		  {
			  playerYPosition -= 1;

		  }
	  }


    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_ball */
/**
* @brief Function implementing the ballControl thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ball */
void ball(void *argument)
{
  /* USER CODE BEGIN ball */
  /* Infinite loop */
	ballXPosition = 50;
	ballYPosition = 30;

	ballXDirection = 1;
	ballYDirection = 1;

	ballXVelocity = -1;
	ballYVelocity = 1;

	uint16_t leftEdge = 0 + paddleWidth;
	uint16_t rightEdge = SSD1306_WIDTH - paddleWidth;
	uint16_t maxBounceAngle = 75;


  for(;;)
  {

//! or not function needed here

	  uint16_t centralizedBallYPosition = ballYPosition + halfBallRadius;

	  //Player
	  if(ballXPosition - ballRadius <= leftEdge)
	  {
		 if((centralizedBallYPosition >= playerYPosition) && (centralizedBallYPosition <= playerYPosition + paddleLength))
		 {

			 //Calculate if on right or left side of paddle and how far along
			 //uint16_t normalizedBallPosition = playerYPosition + paddleLength - centralizedBallYPosition;

			 int16_t relativeIntersectY = (playerYPosition+(paddleLength/2)) - centralizedBallYPosition;
			 //10 to -10

			 double normalizedRelativeIntersectionY = ((double)relativeIntersectY/((double)paddleLength/2));
			 double bounceAngle = (double)normalizedRelativeIntersectionY * (double)maxBounceAngle;


			 ballXDirection = ballXVelocity*cos(bounceAngle);
			 ballYDirection = ballYVelocity*-sin(bounceAngle);

/*
			 if(normalizedBallPosition < paddleLength/2)
			 {
				 //Bottom of the paddle

			 }
			 else if(normalizedBallPosition > paddleLength/2)
			 {
				//TOP of the paddle

			 }
			 else //equal
			 {
				 //Middle of the paddle


			 }
		//	 double angleCalc = (double)normalizedBallPosition/(double)paddleLength;

*/


		 //	ballXDirection *= -1;
		 }


	  }

	  //Opponent
	  else if(ballXPosition + ballRadius >= rightEdge)
	  {

		 if((centralizedBallYPosition >= opponentYPosition) && (centralizedBallYPosition <=opponentYPosition + paddleLength))
		 {

			 ballXDirection *= -1;
		 }

	  }




	  ballXPosition = ballXPosition + (ballXDirection * ballXVelocity);
	  ballYPosition = ballYPosition + (ballYDirection * ballYVelocity);



	  if(ballXPosition <=0 )
	  {

		  opponentScored = 1;

	  }


	  if(ballXPosition >= 140)
	  {
		  playerScored = 1;

	  }

    osDelay(1);
  }
  /* USER CODE END ball */
}

/* USER CODE BEGIN Header_oponentPaddle */
/**
* @brief Function implementing the opponent thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_oponentPaddle */
void oponentPaddle(void *argument)
{
  /* USER CODE BEGIN oponentPaddle */
  /* Infinite loop */
  for(;;)
  {

	  opponentYPosition = ballYPosition;


    osDelay(1);
  }
  /* USER CODE END oponentPaddle */
}

/* USER CODE BEGIN Header_DrawScreen */
/**
* @brief Function implementing the Draw thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_DrawScreen */
void DrawScreen(void *argument)
{
  /* USER CODE BEGIN DrawScreen */


	// uint16_t PlayerPaddleMoved;

  /* Infinite loop */
  for(;;)
  {
	  SSD1306_Clear();

	  //if(PlayerPaddleMoved != playerBarYPosition) // to not make it flicer if its just still
	  //{playerScored = 0;
	  uint16_t opponentScored = 0;

	  if(playerScored)
	  {
		  SSD1306_GotoXY(0,0);

		  SSD1306_Puts("Player scored", &Font_11x18, 1);

		  //HAL_Delay(3000);
	  }
	  else if(opponentScored)
	  {
		  SSD1306_GotoXY(0,0);

		  SSD1306_Puts("Opponent Scored", &Font_11x18, 1);

		  //HAL_Delay(3000);

	  }
	  else
	  {
		  SSD1306_DrawRectangle(0, playerYPosition, paddleWidth, paddleLength, SSD1306_COLOR_WHITE);

		  SSD1306_DrawRectangle(SSD1306_WIDTH - paddleWidth, opponentYPosition, paddleWidth, paddleLength, SSD1306_COLOR_WHITE);

		  SSD1306_DrawCircle(ballXPosition, ballYPosition, ballRadius, SSD1306_COLOR_WHITE);

	  }
	  SSD1306_UpdateScreen();
	  osDelay(1);
  }
  /* USER CODE END DrawScreen */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

