#include <stdint.h>

#include "saxon.h"

volatile char globalC = 'b';

void main() {
	volatile uint32_t a = 1, b = 2, c = 3;
	uint32_t result = 0;
    GPIO_USER->OUTPUT_ENABLE = 0x000000FF;
    GPIO_USER->OUTPUT = 0x00000000;

    globalC+=1;
    UART_A->DATA = globalC;

    uint32_t counter = 0;
    while(1){
        if(counter++ == 10000){
            GPIO_USER->OUTPUT = GPIO_USER->OUTPUT + 1;
            counter = 0;
        }
        while(UART_A->STATUS >> 24){ //UART RX interrupt
            UART_A->DATA = (UART_A->DATA) & 0xFF;
        }
    }
}
