#include "comunicacion.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h> 

/** --- VARIABLES PRIVADAS (static volatile) ---
 * volatile: Porque cambian en la ISR
 * static: Para que nadie fuera de este archivo las toque
 * 
 * 
 * 
 */  

static volatile char buffer_rx[50]; 
static volatile int flag_listo = 0;
static volatile int indice = 0;

FILE uart_io = FDEV_SETUP_STREAM(escribir_UART, leer_UART, _FDEV_SETUP_RW);

void UART_init(uint32_t baudrate, uint8_t Tx, uint8_t Rx){
    UBRR0 = F_CPU/16/baudrate-1;

    if(Tx) UCSR0B |= (1 << TXEN0);
    
    if(Rx){
        UCSR0B |= (1 << RXEN0);   
        UCSR0B |= (1 << RXCIE0); 
    }

    UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);

    stdout = stdin = &uart_io;
}

// --- INTERRUPCIÓN (ISR) ---
ISR(USART_RX_vect){
    char c = UDR0;
    if (c == '\r') { 
        buffer_rx[indice] = '\0'; 
        flag_listo = 1;        
        indice = 0;            
    } else {
        buffer_rx[indice] = c;    
        if(indice < 49) indice++; 
    }
}

// --- FUNCIONES PÚBLICAS ---


int escribir_UART(char c, FILE *stream){
    if (c == '\n') escribir_UART('\r', stream);
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
    return 0;
}

int leer_UART(FILE *stream){
    while(!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

int UART_flag(void) {
    return flag_listo;
}


void UART_Getter(char* buffer_usuario) {
    // Seccion critica: Deshabilitar interrupciones
    cli();
    strcpy(buffer_usuario, (char*)buffer_rx); 
    flag_listo = 0;
    indice = 0;
    sei(); // Rehabilitar interrupciones
}