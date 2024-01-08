/*
 * i2c.h
 *
 *  Created on: 21/12/2023
 *      Author: Goncalo
 */

#ifndef I2C_H_
#define I2C_H_

#include "LCD.h"

#define SLAVE_ADDR 0x48
#define TEMPERATURE_ADDR 0x00
#define CONFIGURATION_ADDR 0x01

#define CONFIGURATION_DATA 0x60

void I2C_Init (void);
uint16_t READ_temperature (void);
void Configure_Sensor (void);
float temp_get (uint16_t data);
void reverse(char* str, int len);
int intToStr(int x, char str[], int d);
void ftoa(float n, char* res, int afterpoint);

#endif /* I2C_H_ */
