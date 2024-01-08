/*
 * LCD.h
 *
 *  Created on: 18/12/2023
 *      Author: Goncalo
 */

#ifndef LCD_H_
#define LCD_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_gpio.h"
#include "driverlib/fpu.h"
#include "driverlib/rom.h"

#define RS GPIO_PIN_0
#define EN GPIO_PIN_1

#define D0 GPIO_PIN_4
#define D1 GPIO_PIN_5
#define D2 GPIO_PIN_6
#define D3 GPIO_PIN_7

#define LCD_DATA_PINS  D3 | D2 | D1 | D0
#define LDC_DATA_PORT GPIO_PORTA_BASE
#define LDC_DATA_PERIPH SYSCTL_PERIPH_GPIOA

#define LCD_CONTROL_PINS RS | EN
#define LDC_CONTROL_PORT GPIO_PORTB_BASE
#define LDC_CONTROL_PERIPH SYSCTL_PERIPH_GPIOB

void LCD_Command(unsigned char c);
void LCD_Clear(void);
void LCD_init(void);
void LCD_Pulse(void);
void LCD_Write_c(unsigned char d);
void LCD_print(char * input);
void LCD_Cursor(uint8_t col);



#endif /* LCD_H_ */
