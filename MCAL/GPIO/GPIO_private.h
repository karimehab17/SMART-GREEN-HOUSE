#ifndef GPIO_PRIVATE_H
#define GPIO_PRIVATE_H

#include "../../LIB/STD_TYPES.h"


/* =========================================================
 *                      PORT A Registers
 * ========================================================= */

#define GPIO_PINA_REG    (*(volatile uint8*)0x39U)
#define GPIO_DDRA_REG    (*(volatile uint8*)0x3AU)
#define GPIO_PORTA_REG   (*(volatile uint8*)0x3BU)


/* =========================================================
 *                      PORT B Registers
 * ========================================================= */

#define GPIO_PINB_REG    (*(volatile uint8*)0x36U)
#define GPIO_DDRB_REG    (*(volatile uint8*)0x37U)
#define GPIO_PORTB_REG   (*(volatile uint8*)0x38U)


/* =========================================================
 *                      PORT C Registers
 * ========================================================= */

#define GPIO_PINC_REG    (*(volatile uint8*)0x33U)
#define GPIO_DDRC_REG    (*(volatile uint8*)0x34U)
#define GPIO_PORTC_REG   (*(volatile uint8*)0x35U)


/* =========================================================
 *                      PORT D Registers
 * ========================================================= */

#define GPIO_PIND_REG    (*(volatile uint8*)0x30U)
#define GPIO_DDRD_REG    (*(volatile uint8*)0x31U)
#define GPIO_PORTD_REG   (*(volatile uint8*)0x32U)


#endif /* GPIO_PRIVATE_H */