/*
 * menu_task.c
 *
 *  Created on: 04/01/2024
 *      Author: Goncalo
 */
#include "priorities.h"
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



#include "utils/uartstdio.h"


#include "tempo.h"


#define MENUTASKSTACKSIZE        500

#define MENU_ITEM_SIZE           sizeof(char)
#define MENU_QUEUE_SIZE          10

xSemaphoreHandle MENUSemaphore_I2C;
xSemaphoreHandle MENUSemaphore_UART;


xQueueHandle MENUQueue;
extern xQueueHandle I2CQueue;
extern xQueueHandle LCDQueue;
extern xQueueHandle UARTQueue;

bool tmp_update = 1;

extern int hora;
extern int min;
extern int seg;

extern int hora_final, minuto_final, segundo_final;


static void
MENUTask(void *pvParameters)
{
    int conta = 0;
    char tecla;
    char packet_number[10];
    char string [20];
    uart_tecla send;
    int j = 0;
    int position[8] = {6,7,9,10,12,13,14,15};
    int position2[6] = {6,7,9,10,12,13};
    char timestr[20]= "Data: DD/MM/AAAA";
    char horastr[18]= "Hora: HH:MM:SS";


    if(xQueueSend(LCDQueue, &timestr, portMAX_DELAY) != pdPASS)
    {
        while(1)
        {
        }
    }

    do{
        for(j = 0; j<8; j++){
            if(xQueueReceive(MENUQueue, &tecla, portMAX_DELAY) != pdPASS)
            {
                while(1)
                {
                }
            }
            if(tecla == 'E'){
                break;
            }
            if(tecla == 'F'){
                j = j - 2;
                if(j<0){
                    j=0;
                }
            }else{

                if(verf_data(j , tecla, timestr) == 1){
                    timestr[position[j]] = tecla;
                }else{
                    if(j==0){
                          j=-1;
                    }else{
                        j = j - 1;
                    }
                }

                if(xQueueSend(LCDQueue, &timestr, portMAX_DELAY) != pdPASS)
                {
                    while(1)
                    {
                    }
                }
            }
        }
    }while (tecla != 'E');

    if(xQueueSend(LCDQueue, &horastr, portMAX_DELAY) != pdPASS){
        while(1)
        {
        }
    }

    do{
        for(j = 0; j<8; j++){
            if(xQueueReceive(MENUQueue, &tecla, portMAX_DELAY) != pdPASS) {
                while(1)
                {
                }
            }

            if(tecla == 'E'){
                break;
            }
            if(tecla == 'F'){
                j = j - 2;
                if(j<0){j=0;}
            }else{
                if(verf_hora(j , tecla, horastr) == 1){
                horastr[position2[j]] = tecla;
                }else{
                    if(j==0){
                        j=-1;
                    }else{
                        j = j - 1;
                    }
                }

                if(xQueueSend(LCDQueue, &horastr, portMAX_DELAY) != pdPASS)
                {
                    while(1)
                    {
                    }
                }
            }
        }
    }while (tecla != 'E');

    guarda_hora(horastr, position2);

    sprintf (string," Press A to Start");
    if(xQueueSend(LCDQueue, &string, portMAX_DELAY) !=pdPASS) {
        while(1)
        {
        }
    }

    do{
        xQueueReceive(MENUQueue, &tecla, portMAX_DELAY);
    }while (tecla != 'A');

    xSemaphoreGive(MENUSemaphore_I2C);
    xSemaphoreGive(MENUSemaphore_UART);

    while (1){
        if(xQueueReceive(MENUQueue, &tecla, portMAX_DELAY) == pdPASS){
            if ((tecla == 'C' || tecla == 'D') ){
                tmp_update = 0;
                int x;
                send.botao = tecla;

                for (x=0; x<10; ++x){
                    packet_number [x] = '\0';
                }

                sprintf (string, "Packet N=");
                if(xQueueSend(LCDQueue, &string, portMAX_DELAY) !=pdPASS) {
                    while(1)
                    {
                    }
                }

                for(x=0; x<11 || tecla == 'E'; ++x){
                    char msg [20];

                    if(xQueueReceive(MENUQueue, &tecla, portMAX_DELAY) == pdPASS){
                        if (tecla>= '0' && tecla <='9' && x<10){

                            packet_number[x]= tecla;
                            sprintf (msg, "Packet N = %s",packet_number);
                            if(xQueueSend(LCDQueue, &msg, portMAX_DELAY) !=pdPASS) {
                               while(1)
                               {
                               }
                            }
                            vTaskDelay(pdMS_TO_TICKS(10));
                        }else if (tecla == 'E'){
                        strcpy (send.npacket, packet_number);
                            if(xQueueSend(UARTQueue, &send, portMAX_DELAY) !=pdPASS) {
                                while(1)
                                {
                                }
                            }
                            break;
                        }else{
                            sprintf (string,"Error: Packet Number");
                            if(xQueueSend(LCDQueue, &string, portMAX_DELAY) !=pdPASS) {
                                while(1)
                                {
                                }
                            }
                            vTaskDelay(pdMS_TO_TICKS(500));
                            tmp_update=1;
                            break;
                        }
                    }else{
                        break;
                    }
                }
            }

            if (tecla == 'B'){
                send.botao = tecla;

                tmp_update = 0;
                if(xQueueSend(UARTQueue, &send, portMAX_DELAY) !=pdPASS) {
                    while(1)
                    {
                    }
                }
            }

            if (tecla == '2'){
                conta = 0;
                tmp_update = 0;

                while(1){
                    junta_hora(horastr, position2);

                    if(xQueueSend(LCDQueue, &horastr, portMAX_DELAY) != pdPASS)
                    {
                        while(1)
                        {
                        }
                    }

                    vTaskDelay(pdMS_TO_TICKS(300));
                    conta++;

                    if(conta == 5){
                        tmp_update =1;
                        break;
                    }
                }
            }

            if (tecla == '3'){
                tmp_update = 0;

                if(xQueueSend(LCDQueue, &timestr, portMAX_DELAY) != pdPASS)
                {
                    while(1)
                    {
                    }
                }
                vTaskDelay(pdMS_TO_TICKS(500));
                tmp_update =1;
            }
        }
    }
}

uint32_t MENUTaskInit(void)
{

    vSemaphoreCreateBinary(MENUSemaphore_I2C);
    xSemaphoreTake(MENUSemaphore_I2C, portMAX_DELAY);

    vSemaphoreCreateBinary(MENUSemaphore_UART);
    xSemaphoreTake(MENUSemaphore_UART, portMAX_DELAY);

    MENUQueue = xQueueCreate(MENU_QUEUE_SIZE, MENU_ITEM_SIZE);

    if(xTaskCreate(MENUTask, (const portCHAR *)"MENU", MENUTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_MENU_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}

