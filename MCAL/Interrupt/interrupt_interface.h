#ifndef INTERRUPT_INTERFACE_H
#define INTERRUPT_INTERFACE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * MCAL INTERRUPT — public API for ATmega32 global I-bit and EXTI0/1/2.
 * Include this header from HAL, Logic, and main. Do not include INTERRUPT_private.h there.
 *
 * Pins: INT0 = PD2, INT1 = PD3, INT2 = PB2 (input, usually pull-up).
 */

#include "STD_TYPES.h"
#include "INTERRUPT_private.h"
#include "stddef.h"

/* Callback type for external interrupt handlers */
typedef void (*EXTI_CallbackType)(void);
extern EXTI_CallbackType EXTI_pfCallBackArr[3];

/* ---------------- External interrupt sources ---------------- */
#define EXTI_INT0             0u
#define EXTI_INT1             1u
#define EXTI_INT2             2u

/* ---------------- Sense control (INT0 / INT1 via MCUCR) ---------------- */
#define EXTI_LOW_LEVEL        0u    /* INT0 / INT1 only */
#define EXTI_ANY_CHANGE       1u    /* INT0 / INT1 only */
#define EXTI_FALLING_EDGE     2u    /* INT0 / INT1 / INT2 */
#define EXTI_RISING_EDGE      3u    /* INT0 / INT1 / INT2 */

/*
 * Description : Set the global interrupt flag (I-bit, sei).
 */
STD_ReturnType INTERRUPT_EnableGlobal(void);
/*
 * Description : Clear the global interrupt flag (cli).
 */
STD_ReturnType INTERRUPT_DisableGlobal(void);
/*
 * Description : Configure INT0, INT1, or INT2 sense bits. Does not enable the source.
 *               INT2 accepts only falling or rising edge.
 */
STD_ReturnType EXTI_SetSense(uint8 Copy_u8Int, uint8 Copy_u8Sense);

/*
 * Description : Clear the matching flag in GIFR (write 1), then set the enable
 *               bit in GICR (INT0 / INT1 / INT2). Call INTERRUPT_EnableGlobal after this.
 */
STD_ReturnType EXTI_Enable(uint8 Copy_u8Int);

/*
* Description : Clear the GICR enable bit for INT0, INT1, or INT2.
 */
STD_ReturnType EXTI_Disable(uint8 Copy_u8Int);
/*
 * Description : Clear a stale INTF0 / INTF1 / INTF2 flag (write 1 to GIFR).
 */
STD_ReturnType EXTI_ClearFlag(uint8 Copy_u8Int);
/*
 * Description : Register the function the ISR calls when the source fires.
 *               Register it before EXTI_Init, so no edge can arrive with no
 *               handler in place. Registering again replaces the old one.
 * Parameters  : Copy_u8Int      — EXTI_INT0 / EXTI_INT1 / EXTI_INT2.
 *               Copy_pfCallback — void function taking void, must not be NULL.
 * Return      : E_NOK for an unknown source or a NULL function pointer.
 */
STD_ReturnType EXTI_SetCallback(uint8 Copy_u8Int, EXTI_CallbackType Copy_pfCallback);


#endif /* INTERRUPT_INTERFACE_H */
