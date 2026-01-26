#include <comunicacion.h>
#include <avr/io.h>
#include <stdio.h>

FILE uart_io = FDEV_SETUP_STREAM(escribir_UART, leer_UART, _FDEV_SETUP_RW);

void UART_init(uint32_t baudrate, uint8_t Tx, uint8_t Rx){
    UBRR0 = F_CPU/16/baudrate-1;

    if(Tx != 0){
        UCSR0B |= (1 << TXEN0);
    }
    if(Rx != 0){
        UCSR0B |= (1 << RXEN0);
    }

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    // Redirige los flujos estándar (printf/scanf) a la UART configurada
    stdout = stdin = &uart_io;
}

int escribir_UART(char c, FILE *stream){
    if (c == '\n') {
        escribir_UART('\r', stream);
    }
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
    return 0;
}

static inline int escribir_NI_UART(char *buffer, int *dato_listo, int *indice){
    char c = UDR0;
    if (c == '\r') { 
        buffer[*indice] = '\0'; 
        *dato_listo = 1;        
        *indice = 0;            
    } else {
        buffer[*indice] = c;    
        if(*indice < 49) (*indice)++;
    }
    return 0;
}

int leer_UART(FILE *stream){
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}