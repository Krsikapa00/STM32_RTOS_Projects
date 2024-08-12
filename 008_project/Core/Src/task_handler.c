/*
 * task_handler.c
 *
 *  Created on: Aug 5, 2024
 *      Author: krsin
 */

#include "main.h"

void extract_command(command_t* cmd);
void process_command(command_t* cmd);

const char * invalid_cmd = "\n\n////Invalid option////\n";


void led_task_handler(void* parameters)
{
	uint32_t cmd_addr;
	command_t *cmd;
	int option = 0;

	const char * menu_msg = "\n========================\n"
							"|       LED Menu       |\n"
							"========================\n"
							"Back to main menu 	---> 0\n"
							"All on				---> 1\n"
							"Switching			---> 2\n"
							"Just External		---> 3\n"
							"Turn off			---> 4\n"
							"Enter your choice here : ";



	while(1)
	{
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		//		Send data to be printed
		xQueueSend(q_print_data, &menu_msg, portMAX_DELAY);
		//	wait until command is sent back
		xTaskNotifyWait(0,0,&cmd_addr, portMAX_DELAY);
		//	Extract command
		cmd = (command_t *)cmd_addr;

		if (cmd->len == 1)
		{
			//			Valid command sent
			option = cmd->payload[0] - 48;
			//	Update the current state and call the next task
			switch (option) {
				case 0:
					currState = sMainMenu;
					xTaskNotify(handle_menu_task, 0, eNoAction);
					break;
				case 1:
					start_led_timers(option);
					break;
				case 2:
					start_led_timers(option);
					break;
				case 3:
					start_led_timers(option);
					break;
				case 4:
					stop_led_timers();
					turn_on_all_leds();
					break;
				default:
					xQueueSend(q_print_data, &invalid_cmd, portMAX_DELAY);
					continue;
					break;
			}
		} else {
			xQueueSend(q_print_data, &invalid_cmd, portMAX_DELAY);
			continue;
		}

		currState = sMainMenu;
		xTaskNotify(handle_menu_task,0, eNoAction);
	}
}

void menu_task_handler(void* parameters)
{
	//	print starting message
	uint32_t cmd_addr;
	command_t *cmd;

	int option = 0;

	const char * menu_msg = "\n========================\n"
							"|         Menu         |\n"
							"========================\n"
							"LED effect    ----> 0\n"
							"Date and time ----> 1\n"
							"Exit          ----> 2\n"
							"Enter your choice here : ";
	while(1)
	{

//		Send data to be printed
		xQueueSend(q_print_data, &menu_msg, portMAX_DELAY);
		//	wait until command is sent back
		xTaskNotifyWait(0,0,&cmd_addr, portMAX_DELAY);
		//	Extract command
		cmd = (command_t *)cmd_addr;

		if (cmd->len == 1)
		{
//			Valid command sent
			option = cmd->payload[0] - 48;
			//	Update the current state and call the next task
			switch (option) {
				case 0:
					currState = sLedEffect;
					xTaskNotify(handle_led_task, 0, eNoAction);
					break;
				case 1:
					currState = sRtcMenu;
					xTaskNotify(handle_rtc_task, 0, eNoAction);
					break;
				case 2:
//					ADD EXIT
					break;
				default:
					break;
			}
		} else {
			xQueueSend(q_print_data, &invalid_cmd, portMAX_DELAY);
			continue;
		}

//		Wait for another task to call this task again
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
	}
}

void rtc_task_handler(void* parameters)
{
	//	print starting message
	uint32_t cmd_addr;
	command_t *cmd;

	int option = 0;

	const char * menu_msg = "\n========================\n"
							"|       RTC Menu       |\n"
							"========================\n"
							"Set Time    ----> 0\n"
							"Set Date 	----> 1\n"
							"Get Date&Time    ----> 2\n"
							"Report    ----> 3\n"

							"Exit          ----> 4\n"
							"Enter your choice here : ";
	while(1)
	{
		//		Do not start anything until this task is notified
		xTaskNotifyWait(0,0,NULL,portMAX_DELAY);
		//		Send starting msg to be printed
		xQueueSend(q_print_data, &menu_msg, portMAX_DELAY);
		//	wait until command is sent back
		xTaskNotifyWait(0,0,&cmd_addr, portMAX_DELAY);
		//	Extract command
		cmd = (command_t *)cmd_addr;
		if (cmd->len == 1)
		{
//			Valid command sent
//			Adjust by ASCI character shift
			option = cmd->payload[0] - 48;
			//	Update the current state and call the next task
			switch (option) {
				case 0:
//					currState = sLedEffect;
//					xTaskNotify(handle_led_task, 0, eNoAction);
					rtc_config_time();
					break;
				case 1:
					rtc_config_date();
					break;
				case 2:
					rtc_get_curr_date_time();
//					xTaskNotify(handle_rtc_task, 0, eNoAction);

//					ADD EXIT
					break;
				case 3:
					if (xTimerIsTimerActive(rtc_timer) == pdFALSE)
					{
						xTimerStart(rtc_timer, portMAX_DELAY);
					} else {
						xTimerStop(rtc_timer, portMAX_DELAY);
					}
//					xTaskNotify(handle_rtc_task, 0, eNoAction);

//					ADD EXIT
					break;
				default:
					break;
			}
		} else {
			xQueueSend(q_print_data, &invalid_cmd, portMAX_DELAY);
			continue;
		}
		currState = sMainMenu;
		xTaskNotify(handle_menu_task,0, eNoAction);

	}
}

void print_task_handler(void* parameters)
{
	uint32_t *msg;

	while(1)
	{
		xQueueReceive(q_print_data, &msg, portMAX_DELAY);
		HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen((char*)msg), HAL_MAX_DELAY);
	}
}

void command_task_handler(void* parameters)
{
	BaseType_t status;
	command_t cmd;
	while(1)
	{
//		Add Notify way indefinitely
		status = xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		if (status == pdTRUE)
		{
			process_command(&cmd);
		}

	}
}

void process_command(command_t* cmd)
{
	//		Process User data (the command) from input_data
	extract_command(cmd);

	//		Use command to notify the required task
	switch (currState) {
		case sMainMenu:
			xTaskNotify(handle_menu_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;
		case sRtcMenu:
			xTaskNotify(handle_rtc_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;
		case sLedEffect:
			xTaskNotify(handle_led_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;
		case sRtcDateConfig:
			xTaskNotify(handle_rtc_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;
		case sRtcReport:
			xTaskNotify(handle_rtc_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;
		case sRtcTimeConfig:
			xTaskNotify(handle_rtc_task, (uint32_t)cmd, eSetValueWithOverwrite);
			break;
		default:
			break;
	}

}


void extract_command(command_t* cmd)
{
	uint8_t indx = 0;
	uint8_t item = 0;
	BaseType_t status;
	do
	{
		status = xQueueReceive(q_input_data, &item, 0);

		if (status == pdTRUE)
		{
			cmd->payload[indx++] = item;
		}
	} while (item != '\n');

	cmd->payload[indx - 1] = '\0';
	cmd->len = indx-1;

}

