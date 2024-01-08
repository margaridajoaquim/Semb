/*
 * keypad.c
 *
 *  Created on: 27/12/2023
 *      Author: Goncalo
 */

#include "keypad.h"

const uint8_t pin[4] = {GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};
volatile char key = 0;

const char keyboard[4][4] = {
{'1', '2', '3', 'F'},
{'4', '5', '6', 'E'},
{'7', '8', '9', 'D'},
{'A', '0', 'B', 'C'}
};


extern xSemaphoreHandle keypadSemaphore;


// PortEIntHandler is an interrupt handler for GPIO Port E, triggered on keypad button press
void PortEIntHandler(void)
{
    static portBASE_TYPE xHigherPriorityTaskWoken;

    // Clear and disable GPIO interrupt for columns
    GPIOIntClear(col_periph, col_pins);
    GPIOIntDisable(col_periph, col_pins);

    // Initialize task-woken flag
    xHigherPriorityTaskWoken = pdFALSE;

    // Give semaphore from ISR to signal keypad task
    xSemaphoreGiveFromISR(keypadSemaphore, &xHigherPriorityTaskWoken);

    // If a higher-priority task was woken, request a context switch
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }
}

// key_pressed returns the character associated with the specified key
char key_pressed(uint8_t col, uint8_t row)
{
    // Adjust row value
    row /= 2;
    if (row == 4)
        row = 3;

    // Return the character from the keyboard matrix
    return keyboard[row][col];
}

// keypad_init initializes the GPIO pins for keypad input and associates an interrupt
void keypad_init(void)
{
    // Enable GPIO peripherals for columns and rows
    SysCtlPeripheralEnable(col_gpio);
    SysCtlPeripheralEnable(row_gpio);

    // Wait until the GPIO peripherals are ready
    while (!SysCtlPeripheralReady(col_gpio));
    while (!SysCtlPeripheralReady(row_gpio));

    // Register PortEIntHandler as the interrupt handler for column GPIO
    GPIOIntRegister(col_periph, PortEIntHandler);

    // Configure column GPIO pins as input
    GPIOPinTypeGPIOInput(col_periph, col_pins);

    // Set GPIO interrupt type to trigger on both rising and falling edges
    GPIOIntTypeSet(col_periph, col_pins, GPIO_BOTH_EDGES);

    // Configure row GPIO pins as output
    GPIOPinTypeGPIOOutput(row_periph, row_pins);

    // Enable GPIO interrupt for columns
    GPIOIntEnable(col_periph, col_pins);

    // Write 1 to all row GPIO pins
    GPIOPinWrite(row_periph, row_pins, row_pins);
}

// detect_key scans the keypad for button press and returns the corresponding character
char detect_key(void)
{
    uint8_t cols;
    char tecla = 'e'; // Default value indicating no key press

    // Write 0 to all row GPIO pins
    GPIOPinWrite(row_periph, row_pins, 0);

    for (cols = 0; cols < 4; cols++) {
        uint8_t reading = 0;

        // Write 1 to the selected column GPIO pin
        GPIOPinWrite(row_periph, pin[cols], pin[cols]);

        // Wait for 2 microseconds
        SysCtlDelay(2 * (SysCtlClockGet() / 3 / 1000000));

        // Read the state of the column GPIO pins
        reading = GPIOPinRead(col_periph, col_pins);

        // If a key is detected, determine the pressed key's character
        if (reading != 0) {
            tecla = key_pressed(cols, reading);
            return tecla;
        }

        // Write 0 to the selected column GPIO pin
        GPIOPinWrite(row_periph, pin[cols], 0);
    }

    return tecla; // Return key
}
