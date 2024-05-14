#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_debug_console.h"
#include "S25FL164.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sound_player.h"
#include "pixy.h"

void TaskPeripheralInit(void * pvParameters);
void TaskImage(void * pvParameters);
void TaskMemRead(void * pvParameters);
void TaskAudioPlay(void * pvParameters);

uint8_t global_flag = 0;

TaskHandle_t xTaskInit;
TaskHandle_t xTaskImage;
TaskHandle_t xTaskMemRead;
TaskHandle_t xTaskAudioPlay;

QueueHandle_t xQueue;

int main( void )
{

	xTaskCreate( TaskPeripheralInit, "TaskPeripheralInit", 100, NULL, 6, &xTaskInit);

	xQueue = xQueueCreate( 5, sizeof( bill_t ) );
	vTaskStartScheduler();


	for( ;; );
}

void TaskPeripheralInit(void * pvParameters)
{

	bill_t billete;
	const TickType_t xDelay250ms = pdMS_TO_TICKS( 10000UL );
	while(1)
	{
		billete = get_bill();
		load_sound(billete);
		play_sound();
		vTaskDelay( xDelay250ms );
	}

}


void TaskImage(void * pvParameters)
{
	bill_t billete;
	while(1)
	{
		if(global_flag)
		{
			vTaskDelete(xTaskAudioPlay);
		}
		billete = get_bill();
		if(billete != 0)
		{

			xQueueSendToBack( xQueue, &billete, 0);
			xTaskCreate( TaskMemRead, "TaskMemRead", 10, NULL, 4, &xTaskMemRead);
			vTaskSuspend(xTaskImage);
		}
	}
}

void TaskMemRead(void * pvParameters)
{
	bill_t bill_value;
	xQueueReceive( xQueue, &bill_value, 0);
	load_sound(bill_value);
	xTaskCreate( TaskAudioPlay, "xTaskAudioPlay", 10, NULL, 3, &xTaskAudioPlay);
	vTaskDelete(xTaskMemRead);

}

void TaskAudioPlay(void * pvParameters)
{
	const TickType_t xDelay250ms = pdMS_TO_TICKS( 10000UL );

	play_sound();
	vTaskDelay( xDelay250ms );

	global_flag = 1;
	vTaskResume(xTaskImage);

}
