/*
 * KEYBOARD_task.c
 *
 *  Created on: 03/01/2024
 *      Author: Goncalo
 */
#include "keypad.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define KEYBOARDTASKSTACKSIZE        128

extern xSemaphoreHandle BUZZERSemaphore;
xSemaphoreHandle keypadSemaphore;

extern xQueueHandle MENUQueue;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif
/**********************************************************************
//                               KEYBOARDTask
// Task responsible for handling keypad input
//
//////////////////////////////////////////////////////
*/
static void KEYBOARDTask(void *pvParameters)
{
    char tecla;

    // Wait for the initial semaphore signal before starting the main loop
    xSemaphoreTake(keypadSemaphore, portMAX_DELAY);

    while (1) {
        // Wait for the keypadSemaphore signal indicating a key press
        xSemaphoreTake(keypadSemaphore, portMAX_DELAY);

        // Introduce a delay of 20 milliseconds for debounce
        vTaskDelay(20 / portTICK_RATE_MS);

        // Check if any key in the column is pressed
        if (GPIOPinRead(col_periph, col_pins) != 0) {
            // Read the pressed key using the detect_key function
            tecla = detect_key();

            // If a valid key is pressed, send it to the MENUQueue
            if (tecla != 'e') {
                if (xQueueSendToBack(MENUQueue, &tecla, portMAX_DELAY) != pdPASS) {
                    // Handle queue send failure (currently set to an infinite loop)
                    while (1) {}
                }

                // Reset the row pins and enable keypad interrupt for the next key press
                GPIOPinWrite(row_periph, row_pins, row_pins);
                GPIOIntEnable(col_periph, col_pins);

                // Wait for keypad interrupt (falling edge) to detect key release
                while (1) {
                    xSemaphoreTake(keypadSemaphore, portMAX_DELAY);

                    // Introduce a delay of 20 milliseconds for debounce
                    vTaskDelay(20 / portTICK_RATE_MS);

                    // Check if the key is no longer pressed
                    if (GPIOPinRead(col_periph, col_pins) == 0) {
                        // Enable keypad interrupt and break the loop
                        GPIOIntEnable(col_periph, col_pins);
                        break;
                    } else {
                        // Reset row pins and enable keypad interrupt for the next key press
                        GPIOPinWrite(row_periph, row_pins, row_pins);
                        GPIOIntEnable(col_periph, col_pins);
                    }
                }
            } else {
                // If 'e' is returned by detect_key, reset row pins and enable keypad interrupt
                GPIOPinWrite(row_periph, row_pins, row_pins);
                GPIOIntEnable(col_periph, col_pins);
            }
        } else {
            // If no key in the column is pressed, reset row pins and enable keypad interrupt
            GPIOPinWrite(row_periph, row_pins, row_pins);
            GPIOIntEnable(col_periph, col_pins);
        }
    }
}

// KEYBOARDTaskInit initializes the KEYBOARD task and sets up the keypadSemaphore
uint32_t KEYBOARDTaskInit(void)
{
    // Create a binary semaphore for synchronization
    vSemaphoreCreateBinary(keypadSemaphore);

    // Initialize keypad GPIO configuration
    keypad_init();

    // Create the KEYBOARD task
    if (xTaskCreate(KEYBOARDTask, (const portCHAR *)"KEYBOARD", KEYBOARDTASKSTACKSIZE, NULL,
                    tskIDLE_PRIORITY + PRIORITY_KEYBOARD_TASK, NULL) != pdTRUE) {
        return(1);
    }

    // Success
    return(0);
}
