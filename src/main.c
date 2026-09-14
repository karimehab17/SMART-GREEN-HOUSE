#include "stddef.h"
#include "../LIB/STD_TYPES.h"
#include "../MCAL/GPIO/GPIO_interface.h"


int main(void)
{
    uint8 switches;

    /* PORTA -> Output */
    GPIO_SetPortDirection(GPIO_PORTA, 0xFF);

    /* PORTB -> Input */
    GPIO_SetPortDirection(GPIO_PORTB, 0x00);

    while (1)
    {
        /* Read switches */
        GPIO_GetPortValue(GPIO_PORTB, &switches);

        /* Display switches on LEDs */
        GPIO_SetPortValue(GPIO_PORTA, switches);
    }

    return 0;
}