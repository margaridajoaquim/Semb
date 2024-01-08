/*
 * PWM.h
 *
 *  Created on: 27/12/2023
 *      Author: Goncalo
 */

#ifndef PWM_H_
#define PWM_H_

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"

void PWM_Init(void);
void ACTIVE_PWM (void);
void DISABLE_PWM (void);

#endif /* PWM_H_ */
