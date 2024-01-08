/*
 * keypad.h
 *
 *  Created on: 27/12/2023
 *      Author: Goncalo
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

#include "inc/hw_memmap.h"
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "keypad.h"

#define col_gpio     SYSCTL_PERIPH_GPIOD
#define col_periph   GPIO_PORTD_BASE
#define col_pins     GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3

#define row_gpio SYSCTL_PERIPH_GPIOC
#define row_periph  GPIO_PORTC_BASE
#define row_pins GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7

extern const uint8_t pin[4];
extern volatile char key;
extern volatile char tecla;
extern const char keyboard[4][4];

extern volatile uint8_t key_flag;
extern volatile uint8_t up;
extern uint32_t time;
extern uint8_t reading;

void int_tick_ms_handler(void);
void PortEIntHandler(void);
char key_pressed(uint8_t col, uint8_t row);
void keypad_init(void);
char detect_key(void);
void keypad_processing(void);


#endif /* KEYPAD_H_ */
