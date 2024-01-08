/*
 * tempo.h
 *
 *  Created on: 07/01/2024
 *      Author: Goncalo
 */

#ifndef TEMPO_H_
#define TEMPO_H_

extern int hora;
extern int min;
extern int seg;

extern int hora_final, minuto_final, segundo_final;

void transforma_hora(char horastr[20], int position2[6]);
void junta_hora (char horastr[20], int position2[6]);
void busca_hora(void);
int guarda_hora( char horastr[20], int position2[6]);
int verf_data(int posicao, char tecla, char timestr[20] );
int verf_hora(int posicao2, char tecla, char horastr[20] );

#endif /* TEMPO_H_ */
