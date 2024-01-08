/*
 * i2c.c
 *
 *  Created on: 21/12/2023
 *      Author: Goncalo
 */
#include "LCD.h"
#include "i2c.h"
#include <string.h>

// Initializes and configures the I2C peripheral for communication
void I2C_Init(void) {
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


// Reads the temperature from the sensor
uint16_t READ_temperature(void) {
    uint16_t data = 0;

    // Set the slave address and initiate a single send operation
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDR, false);
    I2CMasterDataPut(I2C0_BASE, TEMPERATURE_ADDR);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_SINGLE_SEND);
    while (I2CMasterBusy(I2C0_BASE));

    // Set slave address for receiving and initiate burst receive operation
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDR, true);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);
    while (I2CMasterBusy(I2C0_BASE));
    data = I2CMasterDataGet(I2C0_BASE) << 8;

    // Continue burst receive operation and combine received data
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while (I2CMasterBusy(I2C0_BASE));
    data |= I2CMasterDataGet(I2C0_BASE);

    return data;
}


// Configures the sensor with specific settings
void Configure_Sensor(void) {
    // Set the slave address and initiate burst send operation
    I2CMasterSlaveAddrSet(I2C0_BASE, SLAVE_ADDR, false);
    I2CMasterDataPut(I2C0_BASE, CONFIGURATION_ADDR);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
    while (I2CMasterBusy(I2C0_BASE));

    // Complete burst send operation
    I2CMasterDataPut(I2C0_BASE, CONFIGURATION_DATA);
    I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
    while (I2CMasterBusy(I2C0_BASE));
}


float temp_get (uint16_t data){

    //Determine if reading is negative
    if((data && 0x8000) == 0x8000)
    {
        data &= 0x7FFF;

        data = data >> 4;

        return -(data * 0.0625);
    }
    // Working with 12 bits
    data = data >> 4;

    return data * 0.0625; // Converts number to degrees celsius
}

void reverse(char* str, int len)
{
    int i = 0, j = len - 1, temp;

    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;

    while (x) {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char* res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0) {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}


