/*
 * UART_task.c
 *
 *  Created on: 05/01/2024
 *      Author: Goncalo
 */

#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "structs.h"
#include "UART_task.h"
#include "tempo.h"

#define UARTTASKSTACKSIZE        500

#define UART_ITEM_SIZE           sizeof(struct UARTTECLA)
#define UART_QUEUE_SIZE          10

msg_uart buffer[buff_max];

int buffer_line= 0;

extern xSemaphoreHandle MENUSemaphore_UART;
extern xSemaphoreHandle LCDSemaphore;

extern xQueueHandle LCDQueue;
xQueueHandle UARTQueue;

extern bool tmp_update;
extern int hora_final, minuto_final, segundo_final;

void process_buffer (void){
    int i,j;
    int flag=0;

    j=0;
    for (i=4; i<10; ++i){
        if(buffer[buffer_line % buff_max].packet[i] >= '0' && buffer[buffer_line % buff_max].packet[i] <= '9'){
            buffer[buffer_line % buff_max].n_packet[j] = buffer[buffer_line % buff_max].packet[i];
            ++j;
        }else if(buffer[buffer_line % buff_max].packet[i] == ','){
            buffer[buffer_line % buff_max].n_packet[j] = '\0';
            break;
        }
    }

    for (j=0; i< NUM_UART_DATA; ++i){
        if (buffer[buffer_line % buff_max].packet[i] == '-'){
            flag=1;
        }
        if ((buffer[buffer_line % buff_max].packet[i] == ',') && flag == 1){
            break;
        }
        if (flag == 1){
            buffer[buffer_line % buff_max].RSSI[j]=buffer[buffer_line % buff_max].packet[i];
            ++j;
        }
    }

    buffer[buffer_line % buff_max].segundos = segundo_final;
    buffer[buffer_line % buff_max].minutos = minuto_final;
    buffer[buffer_line % buff_max].horas = hora_final;
}

void Receive_MSG (void) {
    int ui32index;

    for(ui32index = 0 ; ui32index < NUM_UART_DATA ; ui32index++){
        buffer[buffer_line % buff_max].packet[ui32index] = UARTCharGet(base_uart);
    }

    busca_hora ();
    process_buffer ();
    ++buffer_line;
}

void UARTIntHandler(void)
{
    uint32_t ui32Status;

    //
    // Get the interrrupt status.
    //
    ui32Status =  UARTIntStatus(base_uart, false);
    //
    // Clear the asserted interrupts.
    //
    UARTIntClear(base_uart, ui32Status);

    Receive_MSG ();
}

void UART_init (void){
    //
    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    //
    FPUEnable();
    FPULazyStackingEnable();

    //
    // Set the clocking to run directly from the crystal.
    //
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
    SYSCTL_XTAL_16MHZ);

    //
    // Enable the peripherals used by this example.
    //
    SysCtlPeripheralEnable(periph_uart);
    SysCtlPeripheralEnable(periph_gpio);

    //
    // Set GPIO A0 and A1 as UART pins.
    //
    GPIOPinConfigure(Rx_gpio);
    GPIOPinConfigure(Tx_gpio);
    GPIOPinTypeUART(porta_gpio_bus, pin_gpio_A0 | pin_gpio_A1);

    //
    // Configure the UART for 115,200, 8-N-1 operation.
    //
    UARTConfigSetExpClk(base_uart,  SysCtlClockGet(), 9600,
    (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
     UART_CONFIG_PAR_NONE));

    IntEnable(INT_UART2);
    UARTIntRegister(UART2_BASE, UARTIntHandler);
    UARTIntEnable(UART2_BASE, UART_INT_RX);
}

bool find_packet (char *packet_number, int *line){
    int i;

    for (i=0; i<buff_max; ++i){
        if (strcmp(packet_number,buffer[i].n_packet)==0){
            *line = i;
            return 1;
        }
    }
    return 0;
}

void leftShift(char *words, int len)
{
    int i;

    for(i = 1; i < NUM_UART_DATA-len; i++)
    {
        words[i - 1] = words[i];
    }
}

// Task to handle UART data and display information on LCD
static void UARTTask(void *pvParameters) {
    uart_tecla received;

    bool exists = false; // Flag to check if a packet exists
    int line; // Line number in the buffer for the found packet
    int i;
    int pos_uart[6] = {9, 10, 12, 13, 15, 16}; // Positions in the UART packet for timestamp extraction
    char hora_uart[18] = "RX_TIME: HH:MM:SS"; // Buffer for formatted timestamp

    // Take the semaphore to synchronize with other tasks
    xSemaphoreTake(MENUSemaphore_UART, portMAX_DELAY);
    // Initialize UART
    UART_init();

    while (1) {
        // Receive data from the UART queue
        if (xQueueReceive(UARTQueue, &received, portMAX_DELAY) == pdPASS) {
            char string[50]; // Buffer for formatted strings

            tmp_update = 0; // Temporary update flag

            if (received.botao == 'B') {
                // Display the number of received packets
                sprintf(string, "N. of packets = %d\0", buffer_line);
            } else {
                // Check if the received packet number exists in the buffer
                exists = find_packet(received.npacket, &line);

                if (exists == false) {
                    // Packet not found in the buffer
                    sprintf(string, "Packet not found\0");
                }

                if (received.botao == 'C' && exists == true) {
                    // Format and display timestamp on LCD
                    transforma_hora(hora_uart, pos_uart);

                    if (xQueueSend(LCDQueue, &hora_uart, portMAX_DELAY) != pdPASS) {
                        while (1) {
                            // Handle failure to send to LCD queue
                        }
                    }

                    // Delay for visual effect
                    int m;
                    for (m = 0; m < 5; m++) {
                        vTaskDelay(pdMS_TO_TICKS(100));
                    }

                    // Display the received packet on LCD with shifting effect
                    strcpy(string, buffer[line].packet);
                    for (i = 0; i < NUM_UART_DATA - 22; ++i) {
                        if (xQueueSend(LCDQueue, &string, portMAX_DELAY) != pdPASS) {
                            while (1) {
                                // Handle failure to send to LCD queue
                            }
                        }
                        vTaskDelay(pdMS_TO_TICKS(100));
                        leftShift(string, i + 1); // Left shift characters for visual effect
                    }
                }

                if (received.botao == 'D' && exists == true) {
                    // Display the RSSI value on LCD
                    sprintf(string, "RSSI=");
                    strncat(string, buffer[line].RSSI, 3);
                }
            }

            // Send the formatted string to the LCD queue
            if (xQueueSend(LCDQueue, &string, portMAX_DELAY) != pdPASS) {
                while (1) {
                    // Handle failure to send to LCD queue
                }
            }

            // Delay for visual effect
            vTaskDelay(pdMS_TO_TICKS(500));
            tmp_update = 1; // Restore temporary update flag
        }
    }
}

uint32_t UARTTaskInit(void)
{


    UARTQueue = xQueueCreate(UART_QUEUE_SIZE, UART_ITEM_SIZE);
    if(xTaskCreate(UARTTask, (const portCHAR *)"UART", UARTTASKSTACKSIZE, NULL,
                   tskIDLE_PRIORITY + PRIORITY_UART_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    //
    // Success.
    //
    return(0);
}




