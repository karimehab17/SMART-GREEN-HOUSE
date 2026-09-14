#include <avr/interrupt.h>

#include "STD_TYPES.h"
#include "GPIO_interface.h"
#include "TIMER_interface.h"

ISR(TIMER0_OVF_vect)
{
    GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, GPIO_HIGH);
}

int main(void)
{
    GPIO_SetPinDirection(GPIO_PORTB, GPIO_PIN0, GPIO_OUTPUT);
    GPIO_SetPinValue(GPIO_PORTB, GPIO_PIN0, GPIO_LOW);

    TIMER0_Init(0); // NORMAL mode
    TIMER0_SetOverflowInterrupt(1u);
    TIMER0_Start(5u); // Prescaler = 1024

    sei();

    while (1)
    {
    }

    return 0;
}