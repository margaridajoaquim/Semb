/*
 * PWM.c
 *
 *  Created on: 27/12/2023
 *      Author: Goncalo
 */

#include "PWM.h"

void PWM_Init(void){
    // Enable the PWM1 peripheral and GPIOF peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    // Configure PF2 as PWM output
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);

    // Configure PWM generator 3 for up-down counting mode
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN |
                        PWM_GEN_MODE_NO_SYNC);

    // Set the period of the PWM signal
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, 64000);

    // Set the initial pulse width (50% duty cycle)
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,
    PWMGenPeriodGet(PWM1_BASE, PWM_GEN_3) / 2);
    // Enable PWM generator 3
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);
}

void ACTIVE_PWM (void){
    // Enable PWM output on PWM_OUT_6 (PF2)
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);
}

void DISABLE_PWM (void){
    // Disable PWM output on PWM_OUT_6 (PF2)
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, false);
}
