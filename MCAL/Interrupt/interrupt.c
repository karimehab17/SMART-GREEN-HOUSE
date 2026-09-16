#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"

#include "INTERRUPT_interface.h"
#include "INTERRUPT_private.h"

#include <stddef.h>
#include <avr/interrupt.h>


/* =========================================================
 *                     Callback Storage
 * ========================================================= */

EXTI_CallbackType EXTI_pfCallBackArr[3] =
{
    NULL,
    NULL,
    NULL
};


/* =========================================================
 *                    Timer0 Scheduler Tick
 * ========================================================= */

volatile uint8 g_tickFlag = 0U;


void INTERRUPT_Timer0CompareCallback(void)
{
    g_tickFlag = 1U;
}


/* =========================================================
 *                    Global Interrupt
 * ========================================================= */

STD_ReturnType INTERRUPT_EnableGlobal(void)
{
    SET_BIT(
        INTERRUPT_SREG_REG,
        INTERRUPT_I_BIT
    );

    return E_OK;
}


STD_ReturnType INTERRUPT_DisableGlobal(void)
{
    CLEAR_BIT(
        INTERRUPT_SREG_REG,
        INTERRUPT_I_BIT
    );

    return E_OK;
}


/* =========================================================
 *                       Sense Control
 * ========================================================= */

STD_ReturnType EXTI_SetSense(
    uint8 Copy_u8Int,
    uint8 Copy_u8Sense)
{
    switch (Copy_u8Int)
    {
        case EXTI_INT0:

            if (Copy_u8Sense > EXTI_RISING_EDGE)
            {
                return E_NOK;
            }

            INTERRUPT_MCUCR_REG &= (uint8)~(
                (1U << INTERRUPT_ISC01_BIT) |
                (1U << INTERRUPT_ISC00_BIT)
            );

            INTERRUPT_MCUCR_REG |= Copy_u8Sense;

            break;

        case EXTI_INT1:

            if (Copy_u8Sense > EXTI_RISING_EDGE)
            {
                return E_NOK;
            }

            INTERRUPT_MCUCR_REG &= (uint8)~(
                (1U << INTERRUPT_ISC11_BIT) |
                (1U << INTERRUPT_ISC10_BIT)
            );

            INTERRUPT_MCUCR_REG |=
                (uint8)(Copy_u8Sense << 2U);

            break;

        case EXTI_INT2:

            if ((Copy_u8Sense != EXTI_FALLING_EDGE) &&
                (Copy_u8Sense != EXTI_RISING_EDGE))
            {
                return E_NOK;
            }

            if (Copy_u8Sense == EXTI_FALLING_EDGE)
            {
                CLEAR_BIT(
                    INTERRUPT_MCUCSR_REG,
                    INTERRUPT_ISC2_BIT
                );
            }
            else
            {
                SET_BIT(
                    INTERRUPT_MCUCSR_REG,
                    INTERRUPT_ISC2_BIT
                );
            }

            break;

        default:
            return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                     Clear EXTI Flag
 * ========================================================= */

STD_ReturnType EXTI_ClearFlag(uint8 Copy_u8Int)
{
    switch (Copy_u8Int)
    {
        case EXTI_INT0:

            SET_BIT(
                INTERRUPT_GIFR_REG,
                INTERRUPT_INTF0_BIT
            );

            break;

        case EXTI_INT1:

            SET_BIT(
                INTERRUPT_GIFR_REG,
                INTERRUPT_INTF1_BIT
            );

            break;

        case EXTI_INT2:

            SET_BIT(
                INTERRUPT_GIFR_REG,
                INTERRUPT_INTF2_BIT
            );

            break;

        default:
            return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                       Enable EXTI
 * ========================================================= */

STD_ReturnType EXTI_Enable(uint8 Copy_u8Int)
{
    switch (Copy_u8Int)
    {
        case EXTI_INT0:

            SET_BIT(
                INTERRUPT_GIFR_REG,
                INTERRUPT_INTF0_BIT
            );

            SET_BIT(
                INTERRUPT_GICR_REG,
                INTERRUPT_INT0_BIT
            );

            break;

        case EXTI_INT1:

            SET_BIT(
                INTERRUPT_GIFR_REG,
                INTERRUPT_INTF1_BIT
            );

            SET_BIT(
                INTERRUPT_GICR_REG,
                INTERRUPT_INT1_BIT
            );

            break;

        case EXTI_INT2:

            SET_BIT(
                INTERRUPT_GIFR_REG,
                INTERRUPT_INTF2_BIT
            );

            SET_BIT(
                INTERRUPT_GICR_REG,
                INTERRUPT_INT2_BIT
            );

            break;

        default:
            return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                       Disable EXTI
 * ========================================================= */

STD_ReturnType EXTI_Disable(uint8 Copy_u8Int)
{
    switch (Copy_u8Int)
    {
        case EXTI_INT0:

            CLEAR_BIT(
                INTERRUPT_GICR_REG,
                INTERRUPT_INT0_BIT
            );

            break;

        case EXTI_INT1:

            CLEAR_BIT(
                INTERRUPT_GICR_REG,
                INTERRUPT_INT1_BIT
            );

            break;

        case EXTI_INT2:

            CLEAR_BIT(
                INTERRUPT_GICR_REG,
                INTERRUPT_INT2_BIT
            );

            break;

        default:
            return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                         Callback
 * ========================================================= */

STD_ReturnType EXTI_SetCallback(
    uint8 Copy_u8Int,
    EXTI_CallbackType Copy_pfCallback)
{
    if (Copy_u8Int > EXTI_INT2)
    {
        return E_NOK;
    }

    if (Copy_pfCallback == NULL)
    {
        return E_NOK;
    }

    EXTI_pfCallBackArr[Copy_u8Int] =
        Copy_pfCallback;

    return E_OK;
}


/* =========================================================
 *                         ISR
 * ========================================================= */

/* INT0: PD2 - Alarm Reset Button */
ISR(INT0_vect)
{
    if (EXTI_pfCallBackArr[EXTI_INT0] != NULL)
    {
        EXTI_pfCallBackArr[EXTI_INT0]();
    }
}


/* INT1: PD3 - Mode Button */
ISR(INT1_vect)
{
    if (EXTI_pfCallBackArr[EXTI_INT1] != NULL)
    {
        EXTI_pfCallBackArr[EXTI_INT1]();
    }
}


/* INT2: PB2 - Not used by Smart Greenhouse */
ISR(INT2_vect)
{
    if (EXTI_pfCallBackArr[EXTI_INT2] != NULL)
    {
        EXTI_pfCallBackArr[EXTI_INT2]();
    }
}


/* Timer0 Compare Match: 10 ms scheduler tick */
ISR(TIMER0_COMP_vect)
{
    g_tickFlag = 1U;
}