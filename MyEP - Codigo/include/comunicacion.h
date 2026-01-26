#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <stdio.h>

extern FILE uart_io;

void UART_init(uint32_t baudrate, uint8_t Tx, uint8_t Rx);
int escribir_UART(char c, FILE *stream);
int leer_UART(FILE *stream);
static inline int escribir_NI_UART(char *buffer, int *dato_listo, int *indice);


#endif // COMUNICACION_H
