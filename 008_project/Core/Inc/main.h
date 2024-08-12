/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "stdio.h"
#include "string.h"
#include "timers.h"
#include <stdint.h>
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
	uint8_t payload[10];
	uint8_t len;
}command_t;

typedef enum
{
	sMainMenu = 0,
	sLedEffect,
	sRtcMenu,
	sRtcTimeConfig,
	sRtcDateConfig,
	sRtcReport
}state_t;


extern RTC_HandleTypeDef hrtc;
extern UART_HandleTypeDef huart2;

extern TaskHandle_t handle_led_task;
extern TaskHandle_t handle_menu_task;
extern TaskHandle_t handle_rtc_task;
extern TaskHandle_t handle_print_task;
extern TaskHandle_t handle_command_task;


extern QueueHandle_t q_input_data;
extern QueueHandle_t q_print_data;

extern TimerHandle_t led_timer[3];
extern TimerHandle_t rtc_timer;

extern volatile command_t currCommand;
extern state_t currState;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void led_task_handler(void* parameters);
void menu_task_handler(void* parameters);
void rtc_task_handler(void* parameters);
void print_task_handler(void* parameters);
void command_task_handler(void* parameters);

void LED_effect_1(void);
void LED_effect_2(void);
void LED_effect_3(void);
void start_led_timers(int n);
void stop_led_timers(void);
void turn_on_all_leds(void);


void rtc_get_curr_date_time_itm(void);
void rtc_get_curr_date_time(void);
void rtc_config_time(void);
void rtc_config_date(void);


/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define DWT_CTRL 		(* (volatile uint32_t *) 0xE0001000)
#define RED_LED_PIN		GPIO_PIN_10
#define YELLOW_LED_PIN	GPIO_PIN_12
#define EXT_LED_PORT	GPIOC
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
