#include <avr/io.h>

#include "STD_TYPES.h"
#include "TIMER_interface.h"
#include "TIMER_private.h"

/*
 * Fallback timer mode definitions for projects where the generated
 * interface header does not provide them yet.
 */
#ifndef TIMER0_NORMAL
#define TIMER0_NORMAL 0u
#define TIMER0_PHASE_CORRECT 1u
#define TIMER0_CTC 2u
#define TIMER0_FAST_PWM 3u
#endif

#ifndef TIMER0_OC_DISCONNECT
#define TIMER0_OC_DISCONNECT 0u
#define TIMER0_OC_TOGGLE 1u
#define TIMER0_OC_NON_INVERT 2u
#define TIMER0_OC_INVERT 3u
#endif

#ifndef TIMER1_NORMAL
#define TIMER1_NORMAL 0u
#define TIMER1_CTC_OCR1A 4u
#define TIMER1_FAST_PWM_8BIT 5u
#define TIMER1_FAST_PWM_ICR1 14u
#endif

#ifndef TIMER_EXT_RISING
#define TIMER_EXT_RISING 7u
#endif

/* =========================================================
 *                         TIMER0
 * ========================================================= */

/*
 * TIMER0_Init
 * 1. Reject an unknown mode.
 * 2. NORMAL          : WGM01=0 WGM00=0
 *    PHASE_CORRECT   : WGM01=0 WGM00=1
 *    CTC             : WGM01=1 WGM00=0
 *    FAST_PWM        : WGM01=1 WGM00=1
 * 3. Do not change CS02:0 here — TIMER0_Start owns the clock.
 */
STD_ReturnType TIMER0_Init(uint8 Copy_u8Mode)
{
    if (Copy_u8Mode > TIMER0_FAST_PWM)
    {
        return E_NOK;
    }

    /* Clear WGM01:WGM00 */
    TIMER0_REG_TCCR0 &= ~((1u << WGM01) | (1u << WGM00));

    switch (Copy_u8Mode)
    {
    case TIMER0_NORMAL:
        /* WGM01=0, WGM00=0 */
        break;

    case TIMER0_PHASE_CORRECT:
        /* WGM01=0, WGM00=1 */
        TIMER0_REG_TCCR0 |= (1u << WGM00);
        break;

    case TIMER0_CTC:
        /* WGM01=1, WGM00=0 */
        TIMER0_REG_TCCR0 |= (1u << WGM01);
        break;

    case TIMER0_FAST_PWM:
        /* WGM01=1, WGM00=1 */
        TIMER0_REG_TCCR0 |= (1u << WGM01) |
                            (1u << WGM00);
        break;

    default:
        return E_NOK;
    }

    return E_OK;
}

/*
 * TIMER0_Start
 * 1. Write CS02:0 from Copy_u8Prescaler.
 * 2. Leave WGM and COM bits as they are.
 */
STD_ReturnType TIMER0_Start(uint8 Copy_u8Prescaler)
{
    if (Copy_u8Prescaler > TIMER_EXT_RISING)
    {
        return E_NOK;
    }

    /* Clear CS02:CS00 */
    TIMER0_REG_TCCR0 &= ~((1u << CS02) |
                          (1u << CS01) |
                          (1u << CS00));

    /* Set new clock source */
    TIMER0_REG_TCCR0 |= Copy_u8Prescaler;

    return E_OK;
}

/*
 * TIMER0_Stop
 * 1. Clear CS02:0 only. TCNT0 is unchanged.
 */
STD_ReturnType TIMER0_Stop(void)
{
    TIMER0_REG_TCCR0 &= ~((1u << CS02) |
                          (1u << CS01) |
                          (1u << CS00));

    return E_OK;
}

/*
 * TIMER0_SetCompareValue
 * 1. Write Copy_u8Value to OCR0.
 */
STD_ReturnType TIMER0_SetCompareValue(uint8 Copy_u8Value)
{
    TIMER0_REG_OCR0 = Copy_u8Value;

    return E_OK;
}

/*
 * TIMER0_SetCompareOutput
 * 1. Write COM01:0.
 */
STD_ReturnType TIMER0_SetCompareOutput(uint8 Copy_u8ComMode)
{
    if (Copy_u8ComMode > TIMER0_OC_INVERT)
    {
        return E_NOK;
    }

    /* Clear COM01:COM00 */
    TIMER0_REG_TCCR0 &= ~((1u << COM01) |
                          (1u << COM00));

    /* COM00 is bit 4, so mode 0..3 maps directly to bits 4..5 */
    TIMER0_REG_TCCR0 |= (Copy_u8ComMode << COM00);

    return E_OK;
}

/*
 * TIMER0_SetOverflowInterrupt / TIMER0_SetCompareInterrupt
 * 1. Set or clear TOIE0 / OCIE0 in TIMSK.
 * 2. Vectors: TIMER0_OVF_vect , TIMER0_COMP_vect.
 *    Global I-bit is INTERRUPT's job.
 */
STD_ReturnType TIMER0_SetOverflowInterrupt(uint8 Copy_u8State)
{
    if (Copy_u8State > 1u)
    {
        return E_NOK;
    }

    if (Copy_u8State == 1u)
    {
        TIMSK_REG |= (1u << TOIE0);
    }
    else
    {
        TIMSK_REG &= ~(1u << TOIE0);
    }

    return E_OK;
}

STD_ReturnType TIMER0_SetCompareInterrupt(uint8 Copy_u8State)
{
    if (Copy_u8State > 1u)
    {
        return E_NOK;
    }

    if (Copy_u8State == 1u)
    {
        TIMSK_REG |= (1u << OCIE0);
    }
    else
    {
        TIMSK_REG &= ~(1u << OCIE0);
    }

    return E_OK;
}

/* =========================================================
 *                         TIMER1
 * ========================================================= */

/*
 * TIMER1_Init / TIMER1_Start / TIMER1_Stop
 * 1. WGM13:0 live in TCCR1A (WGM11:10) and TCCR1B (WGM13:12).
 * 2. CTC on OCR1A is mode 4: WGM13:0 = 0100.
 * 3. CS12:0 are in TCCR1B.
 */
STD_ReturnType TIMER1_Init(uint8 Copy_u8Mode)
{
    if (Copy_u8Mode != TIMER1_NORMAL &&
        Copy_u8Mode != TIMER1_CTC_OCR1A &&
        Copy_u8Mode != TIMER1_FAST_PWM_8BIT &&
        Copy_u8Mode != TIMER1_FAST_PWM_ICR1)
    {
        return E_NOK;
    }

    /* Clear WGM11:WGM10 */
    TIMER1_REG_TCCR1A &= ~((1u << WGM11) |
                           (1u << WGM10));

    /* Clear WGM13:WGM12 */
    TIMER1_REG_TCCR1B &= ~((1u << WGM13) |
                           (1u << WGM12));

    switch (Copy_u8Mode)
    {
    case TIMER1_NORMAL:
        /* WGM13:0 = 0000 */
        break;

    case TIMER1_CTC_OCR1A:
        /* WGM13:0 = 0100 */
        TIMER1_REG_TCCR1B |= (1u << WGM12);
        break;

    case TIMER1_FAST_PWM_8BIT:
        /* WGM13:0 = 0101 */
        TIMER1_REG_TCCR1A |= (1u << WGM10);
        TIMER1_REG_TCCR1B |= (1u << WGM12);
        break;

    case TIMER1_FAST_PWM_ICR1:
        /* WGM13:0 = 1110 */
        TIMER1_REG_TCCR1A |= (1u << WGM11);
        TIMER1_REG_TCCR1B |= (1u << WGM13) |
                             (1u << WGM12);
        break;

    default:
        return E_NOK;
    }

    return E_OK;
}

/*
 * TIMER1_Start
 */
STD_ReturnType TIMER1_Start(uint8 Copy_u8Prescaler)
{
    if (Copy_u8Prescaler > TIMER_EXT_RISING)
    {
        return E_NOK;
    }

    /* Clear CS12:CS10 */
    TIMER1_REG_TCCR1B &= ~((1u << CS12) |
                           (1u << CS11) |
                           (1u << CS10));

    /* Set new clock source */
    TIMER1_REG_TCCR1B |= Copy_u8Prescaler;

    return E_OK;
}

/*
 * TIMER1_Stop
 */
STD_ReturnType TIMER1_Stop(void)
{
    TIMER1_REG_TCCR1B &= ~((1u << CS12) |
                           (1u << CS11) |
                           (1u << CS10));

    return E_OK;
}

/*
 * TIMER1_SetCompareA / TIMER1_SetICR1
 * 1. 16-bit write: high byte first, then low byte
 *    (or assign the 16-bit register).
 */
STD_ReturnType TIMER1_SetCompareA(uint16 Copy_u16Value)
{
    TIMER1_REG_OCR1A = Copy_u16Value;

    return E_OK;
}

STD_ReturnType TIMER1_SetICR1(uint16 Copy_u16Value)
{
    TIMER1_REG_ICR1 = Copy_u16Value;

    return E_OK;
}