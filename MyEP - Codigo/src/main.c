#include <avr/io.h>
#include <util/delay.h>
#include "comunicacion.h"

volatile char buffer_rx[50];
volatile int flag_listo = 0;
volatile int i_rx = 0;


int main(){
  // Settings

  // Work Loop
  while(1){

  }
}

ISR(USART_RX_vect){
    escribir_NI_UART((char *)buffer_rx, (int *)&flag_listo, (int *)&i_rx);
}