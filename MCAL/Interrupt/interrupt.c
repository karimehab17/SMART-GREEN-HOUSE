/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — INTERRUPT.c  (ATmega32 EXTI + global I-bit)
 * Implement every prototype from INTERRUPT_interface.h.
 */

#include "STD_TYPES.h"
#include "INTERRUPT_interface.h"
#include "INTERRUPT_private.h"

#include <avr/io.h>

#ifndef SREG_REG
#define SREG_REG SREG
#endif
#ifndef MCUCR_REG
#define MCUCR_REG MCUCR
#endif
#ifndef MCUCSR_REG
#define MCUCSR_REG MCUCSR
#endif
#ifndef GICR_REG
#define GICR_REG GICR
#endif
#ifndef GIFR_REG
#define GIFR_REG GIFR
#endif

#ifndef I_BIT
#define I_BIT 7u
#endif

/*
 * INTERRUPT_EnableGlobal
 * 1. Set SREG I-bit (sei). Return E_OK.
 */
STD_ReturnType INTERRUPT_EnableGlobal(void)
{
    SREG_REG |= (1u << I_BIT);

    return E_OK;
}

/*
 * INTERRUPT_DisableGlobal
 * 1. Clear SREG I-bit (cli). Return E_OK.
 */
STD_ReturnType INTERRUPT_DisableGlobal(void)
{
    SREG_REG &= ~(1u << I_BIT);

    return E_OK;
}

/*
 * EXTI_SetSense
 * 1. Reject an unknown source.
 * 2. INT0 : write ISC01:ISC00 from Copy_u8Sense (0..3).
 * 3. INT1 : write ISC11:ISC10 the same way.
 * 4. INT2 : only EXTI_FALLING_EDGE (ISC2=0) or EXTI_RISING_EDGE (ISC2=1).
 *    Return E_NOK for low-level / any-change on INT2.
 */
STD_ReturnType EXTI_SetSense(uint8 Copy_u8Int, uint8 Copy_u8Sense)
{
    switch (Copy_u8Int)
    {
    case EXTI_INT0:

        if (Copy_u8Sense > EXTI_RISING_EDGE)
        {
            return E_NOK;
        }

        MCUCR_REG &= ~((1u << ISC01) | (1u << ISC00));
        MCUCR_REG |= Copy_u8Sense;

        break;

    case EXTI_INT1:

        if (Copy_u8Sense > EXTI_RISING_EDGE)
        {
            return E_NOK;
        }

        MCUCR_REG &= ~((1u << ISC11) | (1u << ISC10));
        MCUCR_REG |= (Copy_u8Sense << 2);

        break;

    case EXTI_INT2:

        if (Copy_u8Sense != EXTI_FALLING_EDGE &&
            Copy_u8Sense != EXTI_RISING_EDGE)
        {
            return E_NOK;
        }

        if (Copy_u8Sense == EXTI_FALLING_EDGE)
        {
            MCUCSR_REG &= ~(1u << ISC2);
        }
        else
        {
            MCUCSR_REG |= (1u << ISC2);
        }

        break;

    default:
        return E_NOK;
    }

    return E_OK;
}

/*
 * EXTI_ClearFlag
 * 1. Write 1 to INTF0 / INTF1 / INTF2 in GIFR (w1c).
 */
STD_ReturnType EXTI_ClearFlag(uint8 Copy_u8Int)
{
    switch (Copy_u8Int)
    {
    case EXTI_INT0:
        GIFR_REG |= (1u << INTF0);
        break;

    case EXTI_INT1:
        GIFR_REG |= (1u << INTF1);
        break;

    case EXTI_INT2:
        GIFR_REG |= (1u << INTF2);
        break;

    default:
        return E_NOK;
    }

    return E_OK;
}

/*
 * EXTI_Enable
 * 1. Validate the source.
 * 2. Clear the stale flag first, then set INT0/INT1/INT2 in GICR.
 * 3. Order that always works: sense -> clear flag -> enable source -> sei().
 */
STD_ReturnType EXTI_Enable(uint8 Copy_u8Int)
{
    switch (Copy_u8Int)
    {
    case EXTI_INT0:

        GIFR_REG |= (1u << INTF0);
        GICR_REG |= (1u << INT0);
        break;

    case EXTI_INT1:

        GIFR_REG |= (1u << INTF1);
        GICR_REG |= (1u << INT1);
        break;

    case EXTI_INT2:

        GIFR_REG |= (1u << INTF2);
        GICR_REG |= (1u << INT2);
        break;

    default:
        return E_NOK;
    }

    return E_OK;
}

/*
 * EXTI_Disable
 * 1. Clear the matching GICR bit.
 */
STD_ReturnType EXTI_Disable(uint8 Copy_u8Int)
{
    switch (Copy_u8Int)
    {
    case EXTI_INT0:
        GICR_REG &= ~(1u << INT0);
        break;

    case EXTI_INT1:
        GICR_REG &= ~(1u << INT1);
        break;

    case EXTI_INT2:
        GICR_REG &= ~(1u << INT2);
        break;

    default:
        return E_NOK;
    }

    return E_OK;
}