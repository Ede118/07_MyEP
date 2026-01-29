/**
 * @file main.c
 * @author Barrios Retta 14101
 * @brief Archivo principal del proyecto MyEP
 * 
 * ISR(INT0_vect): Interrupción por sobrecorriente. Se encuentra en motor.c
 * ISR(USART_RX_vect): Interrupción por recepción UART. Se encuentra en comunicacion.c
 * 
 */

#include <avr/io.h>
#include <util/delay.h>
#include "comunicacion.h"
#include "motor.h"



char buffer_usuario[50];

int main(){
  // Settings
  UART_init(9600, 1, 1);
  sei();
  printf("Barrios Retta 14101\nFrecuencia Inicial: 10 kHz.\n");
  motor_init(10000);


  // Work Loop
  while(1){
    if(motor_check_imax()){
      printf("IMAX!\nHard Reset requerido.\r\n");
      while(1); // Bloqueo infinito hasta reinicio fisico
    }

    if(UART_flag()){
      UART_Getter(buffer_usuario);
      interpretar_comando(buffer_usuario);
    }
  }
}
