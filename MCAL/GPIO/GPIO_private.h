#ifndef GPIO_PRIVATE_H
#define GPIO_PRIVATE_H

#include "STD_TYPES.h"

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — GPIO private layer (ATmega32)
 * Include this file ONLY from GPIO.c. Never from main, HAL, or Logic.
 */

/* PORT A */
#define GPIO_DDRA_REG   (*(volatile uint8 *)0x3A)
#define GPIO_PORTA_REG  (*(volatile uint8 *)0x3B)
#define GPIO_PINA_REG   (*(volatile uint8 *)0x39)

/* PORT B */
#define GPIO_DDRB_REG   (*(volatile uint8 *)0x37)
#define GPIO_PORTB_REG  (*(volatile uint8 *)0x38)
#define GPIO_PINB_REG   (*(volatile uint8 *)0x36)

/* PORT C */
#define GPIO_DDRC_REG   (*(volatile uint8 *)0x34)
#define GPIO_PORTC_REG  (*(volatile uint8 *)0x35)
#define GPIO_PINC_REG   (*(volatile uint8 *)0x33)

/* PORT D */
#define GPIO_DDRD_REG   (*(volatile uint8 *)0x31)
#define GPIO_PORTD_REG  (*(volatile uint8 *)0x32)
#define GPIO_PIND_REG   (*(volatile uint8 *)0x30)

#endif /* GPIO_PRIVATE_H */