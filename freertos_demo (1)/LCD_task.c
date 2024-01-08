/*
 * LCD_task.c
 *
 *  Created on: 03/01/2024
 *      Author: Goncalo
 */
#include "LCD.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define LCDTASKSTACKSIZE        500

#define LCD_ITEM_SIZE           sizeof(char)
#define LCD_QUEUE_SIZE          50

xSemaphoreHandle LCDSemaphore;
xQueueHandle LCDQueue;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif
//*****************************************************************************
//                          LCD Task
// Continuously process LCD display updates based on the strings received in the LCDQueue.
//
//*****************************************************************************

static void
void LCDTask(void *pvParameters)
{
    // Buffer to store the received string
    char string[50];

    // Infinite loop for the LCD task
    while(1)
    {
        // Check if there is a string in the LCDQueue
        if(xQueueReceive(LCDQueue, &string, 0) == pdPASS){
            // Take the LCD semaphore to ensure exclusive access
            xSemaphoreTake(LCDSemaphore, portMAX_DELAY);

            // Clear the LCD display
            LCD_Clear ();

            // Print the received string on the LCD
            LCD_print(string);

            // Give back the LCD semaphore to release access
            xSemaphoreGive(LCDSemaphore);

            // Reset the LCDQueue if needed
            // xQueueReset(LCDQueue);
        }
    }
}

//*****************************************************************************
//
// Initializes the LCD task.
//
//*****************************************************************************
uint32_t
// LCDTaskInit initializes the LCD task and related resources
uint32_t
int LCDTaskInit(void)
{
    // Create a queue to store strings for LCD display
    LCDQueue = xQueueCreate(LCD_QUEUE_SIZE, 50 * LCD_ITEM_SIZE);

    // Create a binary semaphore for LCD access control
    vSemaphoreCreateBinary(LCDSemaphore);

    // Initialize the LCD display
    LCD_init();

    // Take the LCD semaphore for exclusive access during initialization
    xSemaphoreTake(LCDSemaphore, portMAX_DELAY);

    // Send command to turn on the display without cursor and blinking
    LCD_Command(0x0C);

    // Clear the LCD display
    LCD_Clear();

    // Set the cursor position to the beginning of the display
    LCD_Cursor(0);

    // Give back the LCD semaphore to allow other tasks to access the LCD
    xSemaphoreGive(LCDSemaphore);

    // Create the LCD task
    if (xTaskCreate(LCDTask, (const portCHAR *)"LCD", LCDTASKSTACKSIZE, NULL,
                    tskIDLE_PRIORITY + PRIORITY_LCD_TASK, NULL) != pdTRUE)
    {
        // Return 1 on failure
        return 1;
    }

    // Return 0 on success
    return 0;
}
