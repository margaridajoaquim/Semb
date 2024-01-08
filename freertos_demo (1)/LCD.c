/*
 * LCD.c
 *
 *  Created on: 18/12/2023
 *      Author: Goncalo
 */

#include "LCD.h"

// Function to send a command (c) to the LCD
void LCD_Command(unsigned char c) {
    // Send upper 4 bits of the command to data pins
    GPIOPinWrite(LDC_DATA_PORT, LCD_DATA_PINS, (c & 0xf0));
    // Set RS to 0 for command mode
    GPIOPinWrite(LDC_CONTROL_PORT, RS, 0x00);
    // Enable LCD by setting EN to 1
    GPIOPinWrite(LDC_CONTROL_PORT, EN, EN);

    // Delay for stability
    SysCtlDelay(10000);

    // Disable LCD by setting EN to 0
    GPIOPinWrite(LDC_CONTROL_PORT, EN, 0x00);

    // Delay for stability
    SysCtlDelay(10000);

    // Send lower 4 bits of the command to data pins
    GPIOPinWrite(LDC_DATA_PORT, LCD_DATA_PINS, (c & 0x0f) << 4);
    // Set RS to 0 for command mode
    GPIOPinWrite(LDC_CONTROL_PORT, RS, 0x00);
    // Enable LCD by setting EN to 1
    GPIOPinWrite(LDC_CONTROL_PORT, EN, EN);

    // Short delay
    SysCtlDelay(10);

    // Disable LCD by setting EN to 0
    GPIOPinWrite(LDC_CONTROL_PORT, EN, 0x00);

    // Delay for stability
    SysCtlDelay(10000);
}

// Function to clear the LCD
void LCD_Clear(void){
    // Send clear command (0x01) to the LCD
    LCD_Command(0x01);
    // Delay for stability
    SysCtlDelay(10);
}

// Function to initialize the LCD
void LCD_init(void) {
    // Enable peripherals for data and control
    SysCtlPeripheralEnable(LDC_DATA_PERIPH);
    SysCtlPeripheralEnable(LDC_CONTROL_PERIPH);

    // Configure data and control pins as outputs
    GPIOPinTypeGPIOOutput(LDC_DATA_PORT, LCD_DATA_PINS);
    GPIOPinTypeGPIOOutput(LDC_CONTROL_PORT, LCD_CONTROL_PINS);

    // Delay for stability
    SysCtlDelay(10000);

    // Set RS to 0 for command mode
    GPIOPinWrite(LDC_CONTROL_PORT, RS, 0x00);

    // Send initialization commands to the LCD
    GPIOPinWrite(LDC_DATA_PORT, LCD_DATA_PINS, 0x30);
    GPIOPinWrite(LDC_CONTROL_PORT, EN, EN);
    SysCtlDelay(10);
    GPIOPinWrite(LDC_CONTROL_PORT, EN, 0x00);

    // Additional initialization commands...

    // Set 4-bit mode
    GPIOPinWrite(LDC_DATA_PORT, LCD_DATA_PINS, 0x20);
    GPIOPinWrite(LDC_CONTROL_PORT, EN, EN);
    SysCtlDelay(10);
    GPIOPinWrite(LDC_CONTROL_PORT, EN, 0x00);

    // Delay for stability
    SysCtlDelay(10000);
}

// Function to generate a pulse on the EN (Enable) pin
void LCD_Pulse(void){
    GPIOPinWrite(LDC_CONTROL_PORT , EN, EN);
    // Short delay
    SysCtlDelay(10);
    GPIOPinWrite(LDC_CONTROL_PORT, EN, 0x00);
}

// Function to write a character (d) to the LCD
void LCD_Write_c(unsigned char d) {
    // Send upper 4 bits of the character to data pins
    GPIOPinWrite(LDC_DATA_PORT, LCD_DATA_PINS, (d & 0xf0));
    // Set RS to 1 for data mode
    GPIOPinWrite(LDC_CONTROL_PORT, RS, RS);
    // Generate pulse
    LCD_Pulse();
    // Delay for stability
    SysCtlDelay(10000);

    // Send lower 4 bits of the character to data pins
    GPIOPinWrite(LDC_DATA_PORT, LCD_DATA_PINS, (d & 0x0f) << 4);
    // Set RS to 1 for data mode
    GPIOPinWrite(LDC_CONTROL_PORT, RS, RS);
    // Generate pulse
    LCD_Pulse();
    // Delay for stability
    SysCtlDelay(10000);
}

// Function to print a string (input) to the LCD
void LCD_print(char * input){
    int size = strlen(input), i;
    // Iterate through each character in the string and write to the LCD
    for(i = 0; i < size; i++) {
        LCD_Write_c(input[i]);
    }
}

// Function to set the cursor position on the LCD
void LCD_Cursor(uint8_t col){
    // Send command to set the cursor position based on column
    LCD_Command(0x80 + (col % 20));
    return;
}
