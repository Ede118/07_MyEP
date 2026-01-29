#ifndef COMUNICACION_H
#define COMUNICACION_H

#include <stdio.h>
#include <avr/io.h> 

// Stream externo para printf
extern FILE uart_io;

// Funciones de Configuración
void UART_init(uint32_t baudrate, uint8_t Tx, uint8_t Rx);
int escribir_UART(char c, FILE *stream);
int leer_UART(FILE *stream);

// Funciones de Acceso (Getters)
int UART_flag(void);
void UART_Getter(char* buffer_usuario);

#endif // COMUNICACION_H