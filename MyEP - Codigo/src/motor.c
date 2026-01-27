#include <avr/io.h>
#include <util/delay.h>
#include "motor.h"

#include "comunicacion.h"

static inline uint8_t elegir_prescaler(uint32_t freq){
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
    ICR1 = F_CPU/freq - 1;
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
    uint8_t prescaler;
    prescaler = elegir_prescaler(freq);

    // Top en ICR1
    ICR1 = F_CPU/freq/prescaler - 1;

    // Duty cycle inicial en 50%
    OCR1A = ICR1 / 2;
    OCR1B = ICR1 / 2;
}

void comando_E0(void){
    HAB_LOW;
    printf("ENA: Off\n");
}

void comando_E1(void){
    HAB_HIGH;
    printf("ENA: On\n");
}

void comando_SD(void){
    desconectar_OC1A();
    conectar_OC1B();
    printf("Sentido de giro: Directo\n");
}

void comando_SI(void){
    conectar_OC1A();
    desconectar_OC1B();
    printf("Sentido de giro: Inverso\n");
}

int comando_Dnnn(int dutyCycle){
    if((dutyCycle < 0) || (dutyCycle > 100)){
        printf("Comando Dnnn: Fuera de rango 0-100%%\n");
        return -1;
    }
    OCR1A = ICR1 * dutyCycle / 100;
    OCR1B = ICR1 * dutyCycle / 100;
    printf("Duty Cycle: %d%%\n", dutyCycle);
    return 0;
}

void comando_Pnnnn(uint16_t T_microsec){
    if((T_microsec < 10) || (T_microsec > 1000)){
        printf("Comando Pnnnn: Fuera de rango 10-1000 us\n");
        return;
    }
    TOP_ICR1(1000000/T_microsec);
}

int interpretar_comando(char* comando){
    if(!strcmp(comando[0], ':')) return -1;
    switch(comando[1]){
        case 'E':
            switch(comando[2]){
                case '0':
                    comando_E0();
                    return 0;
                case '1':
                    comando_E1();
                    return 0;
                default:
                    return -1;
            }
        case 'S':
            switch(comando[2]){
                case 'D':
                    comando_SD();
                    return 0;
                case 'I':
                    comando_SI();
                    return 0;
                default:
                    return -1;
            }
        case 'D':
            int dutyCycle = atoi(&comando[2]);
            comando_Dnnn(dutyCycle);
            return 0;
        case 'P':
            uint16_t T_microsec = atoi(&comando[2]);
            comando_Pnnnn(T_microsec);
            return 0;
        default:
            return -1;
    }
}
