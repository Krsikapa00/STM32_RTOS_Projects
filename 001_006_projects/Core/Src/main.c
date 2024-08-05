/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "stdio.h"
#include <stdint.h>


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DWT_CTRL 		(* (volatile uint32_t *) 0xE0001000)
#define RED_LED_PIN		GPIO_PIN_10
#define YELLOW_LED_PIN	GPIO_PIN_12
#define EXT_LED_PORT	GPIOC
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */
static void switch_priority(void);
static void red_led_handler(void* parameters);
static void yellow_led_handler(void* parameters);
//static void onboard_led_handler(void* parameters);
void button_interrupt_handler(void);

//static void btn_press_handler(void* parameters);


extern  void SEGGER_UART_init(uint32_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
TaskHandle_t red_led_handle;
TaskHandle_t yellow_led_handle;
//TaskHandle_t onboard_led_handle;
TaskHandle_t btn_press_handle;

BaseType_t volatile switch_status;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	BaseType_t status;
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
  /* USER CODE BEGIN 2 */
  SEGGER_UART_init(500000);

//  enable the CYCCNT Counter
  DWT_CTRL |= (1 << 0);


//  Start SEGGER
  SEGGER_SYSVIEW_Conf();
//  SEGGER_SYSVIEW_Start();


  status = xTaskCreate(red_led_handler, "task1", 200, NULL, 2, &red_led_handle);
  configASSERT(status == pdPASS);


  status = xTaskCreate(yellow_led_handler, "task2", 200, NULL, 3, &yellow_led_handle);
  configASSERT(status == pdPASS);


  switch_status = 0;

//  Start scheduler
  vTaskStartScheduler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
  GPIO_InitStruct.Pin = USART_TX_Pin|USART_RX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  HAL_GPIO_WritePin(EXT_LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(EXT_LED_PORT, YELLOW_LED_PIN, GPIO_PIN_RESET);


  /*Configure GPIO pin : RED_LED_PIN */
  GPIO_InitStruct.Pin = RED_LED_PIN|YELLOW_LED_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EXT_LED_PORT, &GPIO_InitStruct);


/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void button_interrupt_handler(void)
{

	traceISR_ENTER();
	switch_status = 1;
	traceISR_EXIT();

}


void switch_priority(void)
{
	UBaseType_t task1_Prio, task2_Prio;
	TaskHandle_t curr_task;
	BaseType_t switch_Prio = 0;
	portENTER_CRITICAL();

	if (switch_status)
	{
		switch_status = 0;
		switch_Prio = 1;
	}
	portEXIT_CRITICAL();

	if (switch_Prio)
	{
//		t1 = xTaskGetHandle("task1");
//		t2 = xTaskGetHandle("task2");

		task1_Prio = uxTaskPriorityGet(red_led_handle);
		task2_Prio = uxTaskPriorityGet(yellow_led_handle);

		curr_task = xTaskGetCurrentTaskHandle();
		if (curr_task == red_led_handle)
		{
			vTaskPrioritySet(red_led_handle, task2_Prio);
			vTaskPrioritySet(yellow_led_handle, task1_Prio);
		} else
		{
			vTaskPrioritySet(yellow_led_handle, task1_Prio);
			vTaskPrioritySet(red_led_handle, task2_Prio);
		}

	}



}


//static void btn_press_handler(void* parameters)
//{
//	uint8_t btn_value = 0;
//	uint8_t prev_value = 0;
//
//	UBaseType_t task1_Prio, task2_Prio;
//
//	while(1)
//	{
////		toggle every 1 second
//		btn_value = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
//
//		if (btn_value)
//		{
//			if (!prev_value)
//			{
//				SEGGER_SYSVIEW_PrintfTarget("Button Pressed");
//
//
//
//			}
//		}
//		prev_value = btn_value;
//		vTaskDelay(pdMS_TO_TICKS(10));
//
//	}
//}


static void red_led_handler(void* parameters)
{

	while(1)
	{
//		toggle every 1 second
		SEGGER_SYSVIEW_PrintfTarget("Toggle RED");
		HAL_GPIO_TogglePin(EXT_LED_PORT, RED_LED_PIN);
		HAL_Delay(100);
		switch_priority();

	}
}

static void yellow_led_handler(void* parameters)
{

	while(1)
	{
//		Toggle every 800 ms
		SEGGER_SYSVIEW_PrintfTarget("Toggle YELLOW");
		HAL_GPIO_TogglePin(EXT_LED_PORT, YELLOW_LED_PIN);
		HAL_Delay(1000);
		switch_priority();
	}
}

/*
static void onboard_led_handler(void* parameters)
{
	BaseType_t status;
	while(1)
	{
//		toggle every 400 ms
		SEGGER_SYSVIEW_PrintfTarget("Toggle ONBAORD");
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

		status = xTaskNotifyWait(0, 0, NULL, pdMS_TO_TICKS(400));
		if (status == pdTRUE)
		{
			portENTER_CRITICAL(); //Disable Interrupts while accessing shared variable

			next_LED_handle = NULL;
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET); //Turn on light before suspending
			vTaskSuspend(onboard_led_handle);
			portEXIT_CRITICAL();

		}
	}
}*/

/* USER CODE END 4 */

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
