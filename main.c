/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"

#define Button_Task_PERIODICITY (10U)
#define Button_Released_TRUE (1U)
#define Button_Released_FALSE (0U)

/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )

char runTimeStatBuffer [190];

TaskHandle_t Button_1_Monitor_Handler = NULL;
TaskHandle_t Button_2_Monitor_Handler = NULL;
TaskHandle_t Periodic_Transmitter_Handler = NULL;
TaskHandle_t UART_Receiver_Handler = NULL;
TaskHandle_t Load_1_Simulation_Handler = NULL;
TaskHandle_t Load_2_Simulation_Handler = NULL;

QueueHandle_t xQueue;

char UART_Receiver_St [32];
char Periodic_Transmitter_St [32] = "\n Periodic Message \n";
char Button_1_Falling_St [32] = "\n Button 1 Falling Edge \n";
char Button_1_Rising_St [32] = "\n Button 1 Rising Edge \n";
char Button_2_Falling_St [32] = "\n Button 2 Falling Edge \n";
char Button_2_Rising_St [32] = "\n Button 2 Rising Edge \n";

int Task_1_In_Time = 0, Task_1_Out_Time = 0, Task_1_Total_Time;
int Task_2_In_Time = 0, Task_2_Out_Time = 0, Task_2_Total_Time;
int Task_3_In_Time = 0, Task_3_Out_Time = 0, Task_3_Total_Time;
int Task_4_In_Time = 0, Task_4_Out_Time = 0, Task_4_Total_Time;
int Task_5_In_Time = 0, Task_5_Out_Time = 0, Task_5_Total_Time;
int Task_6_In_Time = 0, Task_6_Out_Time = 0, Task_6_Total_Time;
int System_Time = 0;
int CPU_Load = 0 ;

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/* Task to be created. */
void Button_1_Monitor( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); */
	TickType_t xLastWakeTime = xTaskGetTickCount();
	pinState_t Prev_Button_1_State = PIN_IS_LOW;
	pinState_t Curr_Button_1_State = PIN_IS_LOW ;
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );

    for( ;; )
    {
			Curr_Button_1_State = GPIO_read(PORT_0,PIN0);
			
			if (Curr_Button_1_State == PIN_IS_LOW && Prev_Button_1_State == PIN_IS_HIGH )
			{
				if (xQueue != 0)
				{
					xQueueSend( xQueue, (void *)&Button_1_Falling_St, (TickType_t) 10);
				}
			}
			else if (Curr_Button_1_State == PIN_IS_HIGH && Prev_Button_1_State == PIN_IS_LOW)
			{
				if (xQueue != 0)
				{
					xQueueSend( xQueue, (void *)&Button_1_Rising_St, (TickType_t) 10);
				}
			}
			Prev_Button_1_State = Curr_Button_1_State;
			
			vTaskDelayUntil(&xLastWakeTime, 50 );
    }
}

void Button_2_Monitor( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); */
	TickType_t xLastWakeTime = xTaskGetTickCount();
	pinState_t Prev_Button_2_State = PIN_IS_LOW;
	pinState_t Curr_Button_2_State = PIN_IS_LOW ;
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );

    for( ;; )
    {
			Curr_Button_2_State = GPIO_read(PORT_0,PIN1);
			
			if (Curr_Button_2_State == PIN_IS_LOW && Prev_Button_2_State == PIN_IS_HIGH )
			{
				if (xQueue != 0)
				{
					xQueueSend( xQueue, (void *)&Button_2_Falling_St, (TickType_t) 10);
				}
			}
			else if (Curr_Button_2_State == PIN_IS_HIGH && Prev_Button_2_State == PIN_IS_LOW)
			{
				if (xQueue != 0)
				{
					xQueueSend( xQueue, (void *)&Button_2_Rising_St, (TickType_t) 10);
				}
			}
			Prev_Button_2_State = Curr_Button_2_State;
			
			vTaskDelayUntil(&xLastWakeTime, 50 );
    }
}

void Periodic_Transmitter( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); */
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );

    for( ;; )
    {
			if(xQueue != 0)
			{
				xQueueSend ( xQueue, (void *)&Periodic_Transmitter_St, (TickType_t) 0);
			}
			
			vTaskDelay(100);
    }
}

void UART_Receiver( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); */
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );

    for( ;; )
    {
			if ( xQueueReceive ( xQueue, &UART_Receiver_St, (TickType_t) 0) ==pdTRUE)
			{
				vSerialPutString( UART_Receiver_St, sizeof(UART_Receiver_St) );
			}
			
			vTaskDelayUntil(&xLastWakeTime, 20);
    }
}

void Load_1_Simulation( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); */
	int i;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 5 );

    for( ;; )
    {
			for ( i=0 ; i<33200 ; i++ )
			{
				i=i;
			}
			vTaskDelayUntil(&xLastWakeTime, 10);
    }
}

void Load_2_Simulation( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below. 
    configASSERT( ( ( uint32_t ) pvParameters ) == 1 ); */
	int i;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( void * ) 6 );

    for( ;; )
    {
			for ( i=0 ; i<78330 ; i++ )
			{
				i=i;
			}
			vTaskDelayUntil(&xLastWakeTime, 100);
    }
}

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
	xQueue = xQueueCreate ( 10, sizeof(UART_Receiver_St)) ;
	
	xTaskCreatePeriodic(
                    Button_1_Monitor,       /* Function that implements the task. */
                    "Button 1 Monitor",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    0,/* Priority at which the task is created. */
                    &Button_1_Monitor_Handler,
										50);      /* Used to pass out the created task's handle. */
										
	xTaskCreatePeriodic(
                    Button_2_Monitor,       /* Function that implements the task. */
                    "Button 2 Monitor",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    0,/* Priority at which the task is created. */
                    &Button_2_Monitor_Handler,
										50);      /* Used to pass out the created task's handle. */

	xTaskCreatePeriodic(
                    Periodic_Transmitter,       /* Function that implements the task. */
                    "Periodic Transmitter",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    0,/* Priority at which the task is created. */
                    &Periodic_Transmitter_Handler,
										100);      /* Used to pass out the created task's handle. */

	xTaskCreatePeriodic(
                    UART_Receiver,       /* Function that implements the task. */
                    "UART Receiver",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    0,/* Priority at which the task is created. */
                    &UART_Receiver_Handler,
										20);      /* Used to pass out the created task's handle. */
										
	xTaskCreatePeriodic(
                    Load_1_Simulation,       /* Function that implements the task. */
                    "Load 1 Simulation",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    0,/* Priority at which the task is created. */
                    &Load_1_Simulation_Handler,
										10);      /* Used to pass out the created task's handle. */
										
	xTaskCreatePeriodic(
                    Load_2_Simulation,       /* Function that implements the task. */
                    "Load 2 Simulation",          /* Text name for the task. */
                    configMINIMAL_STACK_SIZE,      /* Stack size in words, not bytes. */
                    ( void * ) 0,    /* Parameter passed into the task. */
                    0,/* Priority at which the task is created. */
                    &Load_2_Simulation_Handler,
										100);      /* Used to pass out the created task's handle. */
	
    /* Create Tasks here */


	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/


