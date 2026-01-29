#include <stdlib.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "motor.h"
#include "comunicacion.h"

// Variable privada para recordar el prescaler actual
static uint32_t current_prescaler = 1;
static volatile uint32_t current_dutyCycle = 50.0;
static volatile uint8_t flag_imax = 0;

static inline uint32_t elegir_prescaler(uint32_t freq){
    // Limpio los bits de prescaler
    TCCR1B &= ~((1<<CS12) | (1<<CS11) | (1<<CS10));

    if(F_CPU/freq/65536 <= 1){
        TCCR1B |= (1<<CS10);
        return 1;
    } else if(F_CPU/freq/65536 <= 8){
        TCCR1B |= (1<<CS11);
        return 8;
    } else if(F_CPU/freq/65536 <= 64){
        TCCR1B |= (1<<CS10) | (1<<CS11);
        return 64;
    } else if(F_CPU/freq/65536 <= 256){
        TCCR1B |= (1<<CS12);
        return 256;
    } else{
        TCCR1B |= (1<<CS10) | (1<<CS12);
        return 1024;
    }
}

static inline void desconectar_OC1A(void){
    TCCR1A &=~( (1 << COM1A0) | (1 << COM1A1) );
}

static inline void desconectar_OC1B(void){
    TCCR1A &=~( (1 << COM1B0) | (1 << COM1B1) );
}

static inline void conectar_OC1A(void){
    TCCR1A &= ~(1 << COM1A0);
    TCCR1A |= (1 << COM1A1);
}

static inline void conectar_OC1B(void){
    TCCR1A &= ~(1 << COM1B0);
    TCCR1A |= (1 << COM1B1);
}

static inline void TOP_ICR1(uint16_t freq){
    // Usa el prescaler actual para calcular el TOP correcto
    ICR1 = (F_CPU / current_prescaler) / freq - 1;
}

void motor_init(uint32_t freq){
    // Definir IN/OUT
    PW1_OUT;
    PW2_OUT;
    HAB_OUT;
    IMAX_IN;

    // Definir estados iniciales
    HAB_LOW;
    IMAX_OFF;

    // Habilitar interrupciones
    sei();
    EICRA |= (3 << ISC00); // Flanco de subida para INT0
    EIMSK |= (1 << INT0);  // Mascara habilitada para INT0

    // Modo 14: Fast PWM ICR1
    TCCR1A |= (1<<WGM11); 
    TCCR1B |= (3<<WGM12);

    // Clear OC1A on Compare Match (Set at Bottom) -> Non-Inverting
    TCCR1A &= ~(1 << COM1A0);
    TCCR1A |= (1 << COM1A1);

    // OC1B desconectado inicialmente
    TCCR1A &= ~(1 << COM1B0);
    TCCR1A &= ~(1 << COM1B1);
    
    // Prescaler al comienzo
    uint32_t prescaler;
    current_prescaler = elegir_prescaler(freq); // Guardamos el prescaler elegido
    prescaler = current_prescaler;

    // Top en ICR1
    ICR1 = F_CPU/freq/prescaler - 1;

    // Duty cycle inicial en 50%
    OCR1A = ICR1 / 2;
    OCR1B = ICR1 / 2;
}

int comando_E0(void){
    HAB_LOW;
    printf("ENA: Off\n");
    return 0;
}

int comando_E1(void){
    HAB_HIGH;
    printf("ENA: On\n");
    return 0;
}

int comando_SD(void){
    desconectar_OC1A();
    conectar_OC1B();
    printf("Sentido de giro: Directo\n");
    return 0;
}

int comando_SI(void){
    conectar_OC1A();
    desconectar_OC1B();
    printf("Sentido de giro: Inverso\n");
    return 0;
}

int comando_Dnnn(uint16_t dutyCycle){
    if((dutyCycle < 0) || (dutyCycle > 100)){
        printf("Comando Dnnn: Fuera de rango 0-100%%\n");
        return -1;
    }
	current_dutyCycle = (uint32_t)dutyCycle;
    OCR1A = (uint32_t)dutyCycle * ICR1 / 100;
    OCR1B = (uint32_t)dutyCycle * ICR1 / 100;
    printf("Duty Cycle: %d%%\n", dutyCycle);
    return 0;
}

int comando_Pnnnn(uint16_t T_microsec){
    if((T_microsec < 10) || (T_microsec > 1000)){
        printf("Comando Pnnnn: Fuera de rango 10-1000 us\n");
        return -1;
    }
    uint32_t new_freq = 1000000/T_microsec;
	TOP_ICR1(new_freq);
	int aux = comando_Dnnn(current_dutyCycle);
	printf("Nueva frecuencia: %lu Hz\n", new_freq);
    return 0;
}

int interpretar_comando(char* comando){
    if(comando[0] != ':') return -1;
    switch(comando[1]){
        case 'E':
            if(comando[2] == '0') return comando_E0();
            else if(comando[2] == '1') return comando_E1();
            else return -1;
        
         case 'S':
            if(comando[2] == 'D') return comando_SD();
            else if(comando[2] == 'I') return comando_SI();
            else return -1;
        
        case 'D': {
            uint16_t dutyCycle = atoi(&comando[2]); 
			dutyCycle /= 10; 
            return comando_Dnnn(dutyCycle);
        }

        case 'P': {
            uint16_t T_microsec = atoi(&comando[2]);
            return comando_Pnnnn(T_microsec);
        }
        
        default:
            return -1;
    }
}

int motor_check_imax(void){
    return flag_imax;
}

ISR(INT0_vect){
    desconectar_OC1A();
    desconectar_OC1B();
    PW1_LOW;
    PW2_LOW;
    HAB_LOW;
    flag_imax = 1;
}