/*
 * buzzer_task.c
 *
 *  Created on: 04/01/2024
 *      Author: Goncalo
 */
#include "PWM.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define BUZZERTASKSTACKSIZE        128

extern xSemaphoreHandle BUZZERSemaphore;

////////////////////////////////////////////////////////////////////////
//                  BUZZER Task
//The task waits for a semaphore to be signaled, activates the PWM
//signal, and then waits for the semaphore again to deactivate the
//PWM signal.
////////////////////////////////////////////////////////////////////////
static void
BUZZERTask(void *pvParameters)
{

    while (1){
        xSemaphoreTake(BUZZERSemaphore, portMAX_DELAY);
        ACTIVE_PWM ();
        xSemaphoreTake(BUZZERSemaphore, portMAX_DELAY);
        DISABLE_PWM ();
    }
}

uint32_t BUZZERTaskInit(void)
{
    PWM_Init();

    if(xTaskCreate(BUZZERTask, (const portCHAR *)"BUZZER", BUZZERTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_BUZZER_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}

