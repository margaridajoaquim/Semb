/*
 * tempo.c
 *
 *  Created on: 07/01/2024
 *      Author: Goncalo
 */

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "structs.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_ints.h"   //hw ints and lcd library have a collision and dont work together
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"  //

#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/hibernate.c"

#include "tempo.h"

int hora;
int min;
int seg;

int ano;
int mes;
int dia;

int hora_final, minuto_final, segundo_final;



#define SEC_MINUTE 60
#define SEC_HOUR 3600

int atoi(char c){
    if (c == '0')
        return 0;
    if (c == '1')
        return 1;
    if (c == '2')
        return 2;
    if (c == '3')
        return 3;
    if (c == '4')
        return 4;
    if (c == '5')
        return 5;
    if (c == '6')
        return 6;
    if (c == '7')
        return 7;
    if (c == '8')
        return 8;
    if (c == '9')
        return 9;

    return 10;
}


void reverse_str(char* str, int len)
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

int citoa(int num, char* str, int base)
{
    int i = 0;
    bool isNegative = false;

    /* Handle 0 explicitly, otherwise empty string is
     * printed for 0 */
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return 0;
    }

    // In standard itoa(), negative numbers are handled
    // only with base 10. Otherwise numbers are
    // considered unsigned.
    if (num < 0 && base == 10) {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // If number is negative, append '-'
    if (isNegative)
        str[i++] = '-';

    str[i] = '\0'; // Append string terminator

    // Reverse the string
    reverse_str(str, i);

    return 0;
}

void setTimeOfDay(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    uint32_t total = minutes*SEC_MINUTE+hours*SEC_HOUR+seconds;
    HibernateRTCSet(total);
}

uint8_t getHours(uint32_t posixTime)
{
    uint32_t hours = posixTime/SEC_HOUR;

    return hours % 24;
}

uint8_t getMinutes(uint32_t posixTime)
{
    uint32_t minutes = posixTime/SEC_MINUTE;

    return minutes % 60;
}

uint8_t getSeconds(uint32_t posixTime) {
    return posixTime % 60;
}

void transforma_hora(char horastr[20], int position2[6]){
    char dummy[2];

    citoa (hora_final , dummy,10);
    if (hora_final == 0){
        dummy[0]='0';
        dummy[1]='0';
    }

    horastr[position2[0]] = dummy[0];
    horastr[position2[1]] = dummy[1];

    citoa (minuto_final ,dummy,10);
    if (minuto_final == 0){
        dummy[0]='0';
        dummy[1]='0';
    }

    horastr[position2[2]] = dummy[0];
    horastr[position2[3]] = dummy[1];

    citoa (segundo_final ,dummy,10);
    if (segundo_final == 0){
        dummy[0]='0';
        dummy[1]='0';
    }
    horastr[position2[4]] = dummy[0];
    horastr[position2[5]] = dummy[1];
}

void junta_hora (char horastr[20], int position2[6]){

    busca_hora();
    transforma_hora(horastr, position2);

}

void busca_hora(void){
    hora_final = getHours(HibernateRTCGet());
    minuto_final = getMinutes(HibernateRTCGet());
    segundo_final = getSeconds(HibernateRTCGet());
}


int relogio(int hora, int min ,int seg){

    SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // RTC Configuration
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateRTCEnable();


    setTimeOfDay(hora, min, seg);   //sets time of day, #todo dinamical

    return 0;
}

int guarda_hora( char horastr[20], int position2[6]){

    hora = atoi(horastr[position2[0]])*10 + atoi(horastr[position2[1]]);
    min = atoi(horastr[position2[2]])*10 + atoi(horastr[position2[3]]);
    seg = atoi(horastr[position2[4]])*10 + atoi(horastr[position2[5]]);

    relogio(hora, min, seg);

    return 0;
}


/*********************************************************************************
 *
 *  Verifica o formato da data
 *
 *
 **********************************************************************************/

int verf_data(int posicao, char tecla, char timestr[20] ){

    if(atoi(tecla) == 10){
        return 0;
    }

    if(posicao > 3){
        return 1;
    }

    if(posicao == 0 && atoi(tecla) < 4){
        return 1;
    }

    if(posicao == 1){
        if(atoi(timestr[6]) == 0 && atoi(tecla) == 0){
            return 0;
        }
        if(atoi(timestr[6]) == 3 && atoi(tecla) < 2 ){
            return 1;
        }
        if(atoi(timestr[6]) < 3 ){
            return 1;
        }
    }

    if(posicao == 2 && atoi(tecla) < 2){
        return 1;
    }

    if(atoi(timestr[9]) == 0 && atoi(tecla) == 0){
        return 0;
    }

    if(atoi(timestr[9]) == 1 && atoi(tecla) < 3 ){
        return 1;
    }

    if(posicao == 3){
        if(atoi(timestr[9]) == 0){
            return 1;
        }
    }

    return 0;
}

/*********************************************************************************
 *
 *  Verifica o formato da hora
 *
 *
 **********************************************************************************/

int verf_hora(int posicao2, char tecla, char horastr[20] ){

    if(atoi(tecla) == 10){
        return 0;
    }

    //hora
    if(posicao2 == 0 && atoi(tecla) < 3){
        return 1;
    }

    if(posicao2 == 1){
        if(atoi(horastr[6]) < 2 ){
            return 1;
        }
        //else
        if(atoi(horastr[6]) == 2 && atoi(tecla) < 5 ){
            return 1;
        }
    }
    //minuto
    if(posicao2 == 2 && atoi(tecla) < 6){
        return 1;
    }
    if(posicao2 == 3){
        if(atoi(horastr[9]) < 5 ){
            return 1;
        }
        //else
        if(atoi(horastr[9]) == 5 && atoi(tecla) < 10 ){
            return 1;
        }
    }
    //segundo
    if(posicao2 == 4 && atoi(tecla) < 6){
        return 1;
    }

    if(posicao2 == 5){
        if(atoi(horastr[12]) < 5 ){
            return 1;
        }
        if(atoi(horastr[12]) == 5 && atoi(tecla) < 10){
            return 1;
        }
    }

    return 0;
}




