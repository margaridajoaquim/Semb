/*
 * I2C_task.c
 *
 *  Created on: 04/01/2024
 *      Author: Goncalo
 */
#include "i2c.h"
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdbool.h>
#include "tempo.h"

#define I2CTASKSTACKSIZE        500

#define I2C_ITEM_SIZE           sizeof(char)
#define I2C_QUEUE_SIZE          10

xSemaphoreHandle I2CSemaphore;
extern xSemaphoreHandle MENUSemaphore_I2C;
extern xSemaphoreHandle MENUSemaphore_UART;
xSemaphoreHandle BUZZERSemaphore;

extern xQueueHandle LCDQueue;
xQueueHandle I2CQueue;

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

extern bool tmp_update;

extern int hora_final, minuto_final, segundo_final;

/ I2C initialization function to configure the I2C peripheral and GPIO pins
static void I2C_Init(void) {
    // Enable and reset the I2C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_I2C0));

    // Enable GPIO ports for I2C SDA and SCL
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);

    // Configure GPIO pins for I2C SDA and SCL
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);

    // Initialize I2C master with the system clock
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);
}

// Task to read temperature from the sensor using I2C
static void I2CTask(void *pvParameters) {
    uint16_t data;
    int flag = 0;
    float temp;
    char string[50];
    char temp1[10];

    // Initialize I2C and configure the sensor
    I2C_Init();
    Configure_Sensor();

    // Take the semaphore to synchronize with other tasks
    xSemaphoreTake(MENUSemaphore_I2C, portMAX_DELAY);

    while (1) {
        // Read temperature from the sensor
        data = READ_temperature();
        temp = temp_get(data);
        ftoa(temp, temp1, 2);

        // Get the current time
        busca_hora();

        // Format the string with time and temperature
        if (segundo_final < 10) {
            sprintf(string, "%d:%d:0%d %s%cC", hora_final, minuto_final, segundo_final, temp1, 223);
        } else {
            sprintf(string, "%d:%d:%d %s%cC", hora_final, minuto_final, segundo_final, temp1, 223);
        }

        // Check temperature conditions and activate the buzzer accordingly
        if (temp >= 25 && flag == 0) {
            xSemaphoreGive(BUZZERSemaphore);
            flag = 1;
        }

        if (temp < 25 && flag == 1) {
            xSemaphoreGive(BUZZERSemaphore);
            flag = 0;
        }

        // Check if LCD update is required and send data to the LCD queue
        if (tmp_update == 1) {
            if (xQueueSend(LCDQueue, &string, portMAX_DELAY) != pdPASS) {
                while (1) {
                }
            }
            vTaskDelay(pdMS_TO_TICKS(300));
        }
    }
}

// Initializes the I2C task, creates the necessary queues and semaphores
uint32_t I2CTaskInit(void) {
    // Create I2C queue and semaphores
    I2CQueue = xQueueCreate(I2C_QUEUE_SIZE, I2C_ITEM_SIZE);
    vSemaphoreCreateBinary(I2CSemaphore);
    vSemaphoreCreateBinary(BUZZERSemaphore);

    // Take the buzzer semaphore initially
    xSemaphoreTake(BUZZERSemaphore, portMAX_DELAY);

    // Create the I2C task
    if (xTaskCreate(I2CTask, (const portCHAR *)"I2C", I2CTASKSTACKSIZE, NULL,
                    tskIDLE_PRIORITY + PRIORITY_I2C_TASK, NULL) != pdTRUE) {
        return 1; // Return error code if task creation fails
    }

    // Success
    return 0;
}
