
#include "main.h"

HAL_StatusTypeDef process_time_date_input(uint8_t * extracted_value, uint32_t input_add, command_t * cmd, int min, int max);

const char * invalid_set_cmd = "\n\nImproper Input, Could not set Time or Date\n";
const char * valid_set_cmd = "\nConfiguration Successful\n";
const char * invalid_entry = "Invalid Entry.\n";

void rtc_get_curr_date_time_itm(void)
{
	RTC_TimeTypeDef currTime;
	RTC_DateTypeDef currDate;

//	Clear the values for Time and Date
	memset(&currTime, 0, sizeof(currTime));
	memset(&currDate, 0, sizeof(currDate));

//	Get the current values
	HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BCD);

	printf("TIME: %d:%d:%d\n", currTime.Hours, currTime.Minutes, currTime.Seconds);
	printf("DATE: %d/%d/%d - %d\n", currDate.Year, currDate.Month, currDate.Date, currDate.WeekDay);

}


void rtc_get_curr_date_time(void)
{
//	Char buffers to hold the values to print
	static char showTime[40];
	static char showDate[40];

//	HAL_StatusTypeDef status;
	RTC_TimeTypeDef currTime;
	RTC_DateTypeDef currDate;

	static char *time = showTime;
	static char *date = showDate;

//	Clear the values for Time and Date
	memset(&currTime, 0, sizeof(currTime));
	memset(&currDate, 0, sizeof(currDate));

//	Get the current values
	HAL_RTC_GetTime(&hrtc, &currTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &currDate, RTC_FORMAT_BCD);

	sprintf((char*)showTime, "TIME: %d:%d:%d\n", currTime.Hours, currTime.Minutes, currTime.Seconds);
	sprintf((char*)showDate, "DATE: %d/%d/%d - %d\n", currDate.Year, currDate.Month, currDate.Date, currDate.WeekDay);


	xQueueSend(q_print_data, &time, portMAX_DELAY);
	xQueueSend(q_print_data, &date, portMAX_DELAY);

}


void rtc_config_time(void)
{

	HAL_StatusTypeDef status;
	uint32_t input_addr;
	command_t input_time_cmd;
	RTC_TimeTypeDef input_time;
	uint8_t extracted_time;


	const char * hours_msg = "\nEnter Hours (0-23):\t";
	const char * minutes_msg = "\nEnter Minutes (0-59):\t";
	const char * seconds_msg = "\nEnter Seconds (0-59):\t";


	HAL_RTC_GetTime(&hrtc, &input_time, RTC_FORMAT_BCD);

	//	Request Hours and process
	xQueueSend(q_print_data, &hours_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_time, input_addr, &input_time_cmd, 0, 23);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		input_time.Hours = extracted_time;
	}
	//	End of hours processing

	//	Request Minutes and process
	xQueueSend(q_print_data, &minutes_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_time, input_addr, &input_time_cmd, 0, 59);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		input_time.Minutes = extracted_time;
	}
	//	End of minutes processing

	//	Request Seconds and process
	xQueueSend(q_print_data, &seconds_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_time, input_addr, &input_time_cmd, 0, 59);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		input_time.Seconds = extracted_time;
	}
	//	End of Seconds processing



	status = HAL_RTC_SetTime(&hrtc, &input_time, RTC_FORMAT_BCD);

	if (status != HAL_OK)
	{
		xQueueSend(q_print_data, &invalid_set_cmd, portMAX_DELAY);
	} else {
		xQueueSend(q_print_data, &valid_set_cmd, portMAX_DELAY);
	}

}


void rtc_config_date(void)
{

	HAL_StatusTypeDef status;
	uint32_t input_addr;
	command_t input_date_cmd;
	RTC_DateTypeDef input_date;
	uint8_t extracted_date;

	const char * year_msg = "\nEnter Year (0-99):\t";
	const char * month_msg = "\nEnter Month (1-12):\t";
	const char * day_msg = "\nEnter Day (1-31):\t";
	const char * weekday_msg = "\nEnter Week Day\n(Mon = 1, Tue = 2... Sun = 7):\t";


	HAL_RTC_GetDate(&hrtc, &input_date, RTC_FORMAT_BCD);

	//	Request Year and process
	xQueueSend(q_print_data, &year_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_date, input_addr, &input_date_cmd, 0, 99);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		input_date.Year = extracted_date;
	}
	//	End of Year processing

	//	TODO: Fix for months
	//	Request Month and process
	xQueueSend(q_print_data, &month_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_date, input_addr, &input_date_cmd, 1, 12);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		switch (extracted_date) {
			case 10:
				extracted_date = (uint8_t)0x10;
				break;
			case 11:
				extracted_date = (uint8_t)0x11;
				break;
			case 12:
				extracted_date = (uint8_t)0x12;
				break;
			default:
				break;
		}
		input_date.Month = extracted_date;
	}
	//	End of minutes processing

	//	Request Day and process
	int max_day = 30;

	switch (input_date.Month) {
		case RTC_MONTH_FEBRUARY:
			max_day = 28;
			break;
		case RTC_MONTH_APRIL:
		case RTC_MONTH_JUNE:
		case RTC_MONTH_SEPTEMBER:
		case RTC_MONTH_NOVEMBER:
			max_day = 31;
			break;
		default:
			break;
	}

	xQueueSend(q_print_data, &day_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_date, input_addr, &input_date_cmd, 1, max_day);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		input_date.Date = extracted_date;
	}
	//	End of Day processing

	//	Request Week Day and process
	xQueueSend(q_print_data, &weekday_msg, portMAX_DELAY);
	xTaskNotifyWait(0,0,&input_addr, portMAX_DELAY);

	status = process_time_date_input(&extracted_date, input_addr, &input_date_cmd, 1, 7);
	if (status == HAL_ERROR)
	{
		xQueueSend(q_print_data, &invalid_entry, portMAX_DELAY);
		return;
	} else {
		input_date.WeekDay = extracted_date;
	}
	//	End of Week Day  processing


	status = HAL_RTC_SetDate(&hrtc, &input_date, RTC_FORMAT_BCD);

	if (status != HAL_OK)
	{
		xQueueSend(q_print_data, &invalid_set_cmd, portMAX_DELAY);
	} else {
		xQueueSend(q_print_data, &valid_set_cmd, portMAX_DELAY);
	}

}



HAL_StatusTypeDef process_time_date_input(uint8_t * extracted_value, uint32_t input_add, command_t * cmd, int min, int max)
{
//	uint8_t extracted_value;
	cmd = (command_t *)input_add;
	if (cmd->len == 1)
	{
//		Put the first two elements
		*extracted_value = (cmd->payload[0] - 48);
	} else if (cmd->len == 2) {
		*extracted_value = ((cmd->payload[0] - 48) * 10) + (cmd->payload[1] - 48);
	} else {
//		Print message saying input is invalid (too long)
		return HAL_ERROR;
	}

	if (*extracted_value < min)
	{
//		Too low of a value. Return false
		return HAL_ERROR;
	}
	if (*extracted_value > max)
	{
//		Too high of a value. Return false
		return HAL_ERROR;
	}

	return HAL_OK;
}






