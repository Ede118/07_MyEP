#include <avr/io.h>
#include <util/delay.h>
#include "comunicacion.h"


char buffer_usuario[50];
int echo = 0;

int main(){
  // Settings
  UART_init(9600, 1, 1);
  sei();
  printf("Barrios Retta 14101\n");

  // Work Loop
  while(1){
    if(UART_flag()){
      UART_Getter(buffer_usuario);
      if(echo) printf("Echo: %s\n", buffer_usuario);
      
    }
  }
}
