#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * MCAL GPIO — public API for ATmega32 ports A/B/C/D.
 * Include this header from HAL, Logic, and main. Do not include GPIO_private.h there.
 */

#include "STD_TYPES.h"

/* ---------------- Ports ---------------- */
#define GPIO_PORTA    0u
#define GPIO_PORTB    1u
#define GPIO_PORTC    2u
#define GPIO_PORTD    3u

/* ---------------- Pins ---------------- */
#define GPIO_PIN0     0u
#define GPIO_PIN1     1u
#define GPIO_PIN2     2u
#define GPIO_PIN3     3u
#define GPIO_PIN4     4u
#define GPIO_PIN5     5u
#define GPIO_PIN6     6u
#define GPIO_PIN7     7u

/* ---------------- Direction ---------------- */
#define GPIO_INPUT           0u
#define GPIO_OUTPUT          1u
#define GPIO_INPUT_PULLUP    2u

/* ---------------- Pin level ---------------- */
#define GPIO_LOW      0u
#define GPIO_HIGH     1u

/*
 * Description : Set one pin as input, output, or input with internal pull-up.
 *               INPUT_PULLUP writes 0 to DDRx and 1 to PORTx.
 */
STD_ReturnType GPIO_SetPinDirection(uint8 Copy_u8Port, uint8 Copy_u8Pin, uint8 Copy_u8Direction);

/*
 * Description : Write HIGH or LOW on an output pin (PORTx bit).
 */
STD_ReturnType GPIO_SetPinValue(uint8 Copy_u8Port, uint8 Copy_u8Pin, uint8 Copy_u8Value);

/*
 * Description : Read the current level from PINx into *Copy_pu8Value (GPIO_LOW / GPIO_HIGH).
 */
STD_ReturnType GPIO_GetPinValue(uint8 Copy_u8Port, uint8 Copy_u8Pin, uint8 *Copy_pu8Value);

/*
 * Description : Toggle one output pin.
 */
STD_ReturnType GPIO_TogglePinValue(uint8 Copy_u8Port, uint8 Copy_u8Pin);

/*
 * Description : Set all 8 pins of a port to input or output (0x00 or 0xFF on DDRx).
 */
STD_ReturnType GPIO_SetPortDirection(uint8 Copy_u8Port, uint8 Copy_u8Direction);

/*
 * Description : Write an 8-bit pattern to PORTx.
 */
STD_ReturnType GPIO_SetPortValue(uint8 Copy_u8Port, uint8 Copy_u8Value);

/*
 * Description : Read PINx into *Copy_pu8Value.
 */
STD_ReturnType GPIO_GetPortValue(uint8 Copy_u8Port, uint8 *Copy_pu8Value);

#endif /* GPIO_INTERFACE_H */
