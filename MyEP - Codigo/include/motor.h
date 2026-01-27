#ifndef MOTOR_H
#define MOTOR_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// B1, B2, B5 como salidas
#define PW1_OUT        (DDRB |= (1<<PB1))     // Salida
#define PW1_HIGH       (PORTB |= (1<<PB1))    // Alto
#define PW1_LOW        (PORTB &=~ (1<<PB1))   // Bajo

#define PW2_OUT        (DDRB |= (1<<PB2))     // Salida
#define PW2_HIGH       (PORTB |= (1<<PB2))    // Alto
#define PW2_LOW        (PORTB &=~ (1<<PB2))   // Bajo

#define HAB_OUT        (DDRB |= (1<<PB5))     // Salida
#define HAB_HIGH       (PORTB |= (1<<PB5))    // Alto
#define HAB_LOW        (PORTB &=~ (1<<PB5))   // Bajo

// D2 como entrada
#define IMAX_IN         (DDRD &=~ (1<<PD2))    // Entrada
#define IMAX_ON         (PORTD |= (1<<PD2))    // Enciende
#define IMAX_OFF        (PORTD &=~ (1<<PD2))   // Apaga

void motor_init(uint32_t freq);

int comando_E0(void);
int comando_E1(void);
int comando_SD(void);
int comando_SI(void);
int comando_Dnnn(int dutyCycle);
int comando_Pnnnn(uint16_t T_microsec);
int motor_check_imax(void);

int interpretar_comando(char* comando);




#endif // MOTOR_H
