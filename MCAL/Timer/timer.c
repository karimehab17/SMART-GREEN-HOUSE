#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"
#include "config.h"

#include "TIMER_interface.h"
#include "TIMER_private.h"


/* =========================================================
 *                     Private Helpers
 * ========================================================= */

static void TIMER0_ClearClockSelect(void)
{
    TIMER0_TCCR0_REG &= (uint8)~(
        (1U << TIMER0_CS02_BIT) |
        (1U << TIMER0_CS01_BIT) |
        (1U << TIMER0_CS00_BIT)
    );
}


static void TIMER2_ClearClockSelect(void)
{
    TIMER2_TCCR2_REG &= (uint8)~(
        (1U << TIMER2_CS22_BIT) |
        (1U << TIMER2_CS21_BIT) |
        (1U << TIMER2_CS20_BIT)
    );
}


/* =========================================================
 *                         TIMER0
 * ========================================================= */

STD_ReturnType TIMER0_Init(uint8 Copy_u8Mode)
{
    if (Copy_u8Mode > TIMER0_FAST_PWM)
    {
        return E_NOK;
    }

    TIMER0_TCCR0_REG &= (uint8)~(
        (1U << TIMER0_WGM01_BIT) |
        (1U << TIMER0_WGM00_BIT)
    );

    switch (Copy_u8Mode)
    {
        case TIMER0_NORMAL:
            break;

        case TIMER0_PHASE_CORRECT:
            SET_BIT(TIMER0_TCCR0_REG, TIMER0_WGM00_BIT);
            break;

        case TIMER0_CTC:
            SET_BIT(TIMER0_TCCR0_REG, TIMER0_WGM01_BIT);
            break;

        case TIMER0_FAST_PWM:
            SET_BIT(TIMER0_TCCR0_REG, TIMER0_WGM01_BIT);
            SET_BIT(TIMER0_TCCR0_REG, TIMER0_WGM00_BIT);
            break;

        default:
            return E_NOK;
    }

    return E_OK;
}


STD_ReturnType TIMER0_Start(uint8 Copy_u8Prescaler)
{
    if (Copy_u8Prescaler > TIMER0_PRESC_1024)
    {
        return E_NOK;
    }

    TIMER0_ClearClockSelect();

    TIMER0_TCCR0_REG |= Copy_u8Prescaler;

    return E_OK;
}


STD_ReturnType TIMER0_Stop(void)
{
    TIMER0_ClearClockSelect();

    return E_OK;
}


STD_ReturnType TIMER0_SetCompareValue(uint8 Copy_u8Value)
{
    TIMER0_OCR0_REG = Copy_u8Value;

    return E_OK;
}


STD_ReturnType TIMER0_SetCompareInterrupt(uint8 Copy_u8State)
{
    if (Copy_u8State > TIMER_INTERRUPT_ENABLE)
    {
        return E_NOK;
    }

    if (Copy_u8State == TIMER_INTERRUPT_ENABLE)
    {
        SET_BIT(TIMER_TIMSK_REG, TIMER0_OCIE0_BIT);
    }
    else
    {
        CLEAR_BIT(TIMER_TIMSK_REG, TIMER0_OCIE0_BIT);
    }

    return E_OK;
}


/* =========================================================
 *                         TIMER2
 * ========================================================= */

STD_ReturnType TIMER2_PWM(uint8 Copy_u8DutyPercent)
{
    uint8 Local_u8OCRValue;

    if (Copy_u8DutyPercent > 100U)
    {
        return E_NOK;
    }

    TIMER2_TCCR2_REG &= (uint8)~(
        (1U << TIMER2_WGM21_BIT) |
        (1U << TIMER2_WGM20_BIT) |
        (1U << TIMER2_COM21_BIT) |
        (1U << TIMER2_COM20_BIT)
    );

    TIMER2_TCCR2_REG |=
        (1U << TIMER2_WGM21_BIT) |
        (1U << TIMER2_WGM20_BIT) |
        (1U << TIMER2_COM21_BIT);

    Local_u8OCRValue =
        (uint8)(((uint16)Copy_u8DutyPercent * 255U) / 100U);

    TIMER2_OCR2_REG = Local_u8OCRValue;

    TIMER2_ClearClockSelect();

    TIMER2_TCCR2_REG |=
        (1U << TIMER2_CS22_BIT) |
        (1U << TIMER2_CS21_BIT);

    return E_OK;
}


STD_ReturnType TIMER2_BuzzerTone(uint16 Copy_u16FrequencyHz)
{
    if (Copy_u16FrequencyHz == 0U)
    {
        return TIMER2_Stop();
    }

    TIMER2_TCCR2_REG &= (uint8)~(
        (1U << TIMER2_WGM21_BIT) |
        (1U << TIMER2_WGM20_BIT) |
        (1U << TIMER2_COM21_BIT) |
        (1U << TIMER2_COM20_BIT)
    );

    TIMER2_TCCR2_REG |=
        (1U << TIMER2_WGM21_BIT) |
        (1U << TIMER2_WGM20_BIT) |
        (1U << TIMER2_COM21_BIT);

    TIMER2_OCR2_REG = 127U;

    TIMER2_ClearClockSelect();

    /*
     * Timer2 is used as the buzzer tone generator.
     * Select the closest available frequency.
     */

    if (Copy_u16FrequencyHz >= 15000U)
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS20_BIT);
    }
    else if (Copy_u16FrequencyHz >= 2000U)
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS21_BIT);
    }
    else if (Copy_u16FrequencyHz >= 700U)
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS21_BIT) |
            (1U << TIMER2_CS20_BIT);
    }
    else if (Copy_u16FrequencyHz >= 350U)
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS22_BIT);
    }
    else if (Copy_u16FrequencyHz >= 170U)
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS22_BIT) |
            (1U << TIMER2_CS20_BIT);
    }
    else if (Copy_u16FrequencyHz >= 70U)
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS22_BIT) |
            (1U << TIMER2_CS21_BIT);
    }
    else
    {
        TIMER2_TCCR2_REG |=
            (1U << TIMER2_CS22_BIT) |
            (1U << TIMER2_CS21_BIT) |
            (1U << TIMER2_CS20_BIT);
    }

    return E_OK;
}


STD_ReturnType TIMER2_Stop(void)
{
    TIMER2_ClearClockSelect();

    TIMER2_TCCR2_REG &= (uint8)~(
        (1U << TIMER2_COM21_BIT) |
        (1U << TIMER2_COM20_BIT)
    );

    TIMER2_OCR2_REG = 0U;

    return E_OK;
}