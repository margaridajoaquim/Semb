/*
 * structs.h
 *
 *  Created on: 04/01/2024
 *      Author: Goncalo
 */

#ifndef STRUCTS_H_
#define STRUCTS_H_

#define NUM_UART_DATA       45

typedef struct UARTTECLA{
    char botao;
    char npacket[10];
}uart_tecla;

typedef struct UART_MSG{
    char packet [NUM_UART_DATA];
    char n_packet [10];
    char RSSI [3];
    int segundos;
    int minutos;
    int horas;
}msg_uart;

#endif /* STRUCTS_H_ */
