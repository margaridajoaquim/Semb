/*
 * UART_task.h
 *
 *  Created on: 05/01/2024
 *      Author: Goncalo
 */

#ifndef UART_TASK_H_
#define UART_TASK_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#define porta_gpio_bus      GPIO_PORTD_BASE
#define porta_gpio          GPIO_PORTF_BASE
#define pin_gpio_A0         GPIO_PIN_6
#define pin_gpio_A1         GPIO_PIN_7
#define Rx_gpio             GPIO_PD6_U2RX
#define Tx_gpio             GPIO_PD7_U2TX
#define base_uart           UART2_BASE
#define periph_uart         SYSCTL_PERIPH_UART2
#define periph_gpio         SYSCTL_PERIPH_GPIOD

extern uint32_t UARTTaskInit(void);

#define NUM_UART_DATA       45
#define buff_max            20

#endif /* UART_TASK_H_ */
