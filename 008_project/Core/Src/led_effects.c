/*
 * led_effects.c
 *
 *  Created on: Aug 5, 2024
 *      Author: krsin
 */

#include "main.h"


void stop_led_timers(void)
{
	BaseType_t status;

	for (int i = 0; i < 3; i++)
	{
//		Stop the timers indefinitely
		status = xTimerStop(led_timer[i], portMAX_DELAY);
		configASSERT(status == pdTRUE)
	}
}

void start_led_timers(int n)
{
	BaseType_t status;
	configASSERT((n <=3) && (n>0));
	stop_led_timers();
	status = xTimerStart(led_timer[n - 1], portMAX_DELAY);
	configASSERT(status == pdTRUE);
}

void turn_on_all_leds(void)
{
	HAL_GPIO_WritePin(EXT_LED_PORT, RED_LED_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EXT_LED_PORT, YELLOW_LED_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

void turn_off_all_leds(void)
{
	HAL_GPIO_WritePin(EXT_LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXT_LED_PORT, YELLOW_LED_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void external_leds_on(void)
{
	HAL_GPIO_WritePin(EXT_LED_PORT, RED_LED_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(EXT_LED_PORT, YELLOW_LED_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void onboard_leds_on(void)
{
	HAL_GPIO_WritePin(EXT_LED_PORT, RED_LED_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(EXT_LED_PORT, YELLOW_LED_PIN, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
}

void LED_effect_1(void)
{
	static int led_flip = 1;

	(led_flip ^= 1) ? turn_off_all_leds(): turn_on_all_leds();
}

void LED_effect_2(void)
{
	static int led_flip = 1;

	(led_flip ^= 1) ? onboard_leds_on(): external_leds_on();
}

void LED_effect_3(void)
{
	static int led_flip = 1;

	(led_flip ^= 1) ? turn_off_all_leds(): external_leds_on();
}













