/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — ADC.c  (ATmega32, 10-bit)
 * Implement every prototype from ADC_interface.h.
 */

#include "STD_TYPES.h"
#include "Math.h"
#include "ADC_interface.h"
#include "ADC_private.h"
#include "stddef.h"

STD_ReturnType ADC_Init(uint8 Copy_u8Ref, uint8 Copy_u8Prescaler)
{
    if ((Copy_u8Ref > ADC_REF_INTERNAL_2V56) ||
    (Copy_u8Prescaler > ADC_PRESC_128))
{
    return E_NOK;
}
    STD_ReturnType local_Status = E_OK;

    /* Reference voltage (REFS1:REFS0) */
    CLEAR_BIT(ADC_ADMUX_REG, ADC_REFS0_BIT);
    CLEAR_BIT(ADC_ADMUX_REG, ADC_REFS1_BIT);
    if (GET_BIT(Copy_u8Ref, 0))
    {
        SET_BIT(ADC_ADMUX_REG, ADC_REFS0_BIT);
    }
    if (GET_BIT(Copy_u8Ref, 1))
    {
        SET_BIT(ADC_ADMUX_REG, ADC_REFS1_BIT);
    }

    /* Right-adjusted result */
    CLEAR_BIT(ADC_ADMUX_REG, ADC_ADLAR_BIT);

    /* Default channel 0 */
    CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX0_BIT);
    CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX1_BIT);
    CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX2_BIT);
    CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX3_BIT);
    CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX4_BIT);

    /* Clock prescaler (ADPS2:ADPS0) */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADPS0_BIT);
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADPS1_BIT);
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADPS2_BIT);
    if (GET_BIT(Copy_u8Prescaler, 0))
    {
        SET_BIT(ADC_ADCSRA_REG, ADC_ADPS0_BIT);
    }
    if (GET_BIT(Copy_u8Prescaler, 1))
    {
        SET_BIT(ADC_ADCSRA_REG, ADC_ADPS1_BIT);
    }
    if (GET_BIT(Copy_u8Prescaler, 2))
    {
        SET_BIT(ADC_ADCSRA_REG, ADC_ADPS2_BIT);
    }

    /* Enable the ADC module */
    SET_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    return local_Status;
}

/*
 * Disables the ADC peripheral (powers it down / stops conversions).
 */
STD_ReturnType ADC_DeInit(void)
{
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    return E_OK;
}

/*
 * Selects the given channel and starts a single conversion (non-blocking).
 */
STD_ReturnType ADC_StartConversion(uint8 uint8Channel)
{
    STD_ReturnType local_Status = E_OK;

    if (uint8Channel >= ADC_NUMBER_OF_CHANNELS)
    {
        local_Status = E_NOK;
    }
    else
    {
        CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX0_BIT);
        CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX1_BIT);
        CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX2_BIT);
        CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX3_BIT);
        CLEAR_BIT(ADC_ADMUX_REG, ADC_MUX4_BIT);
        if (GET_BIT(uint8Channel, 0))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_MUX0_BIT);
        }
        if (GET_BIT(uint8Channel, 1))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_MUX1_BIT);
        }
        if (GET_BIT(uint8Channel, 2))
        {
            SET_BIT(ADC_ADMUX_REG, ADC_MUX2_BIT);
        }

        SET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT);
    }

    return local_Status;
}

/*
 * Reports whether the ADC has finished the conversion that was last started.
 */
uint8 ADC_IsConversionComplete(void)
{
    uint8 uint8Status = ADC_CONVERSION_DONE;

    if (GET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT))
    {
        uint8Status = ADC_CONVERSION_BUSY;
    }

    return uint8Status;
}

/*
 * Reads the 10-bit result of the last completed conversion.
 */
STD_ReturnType ADC_ReadResult(uint16 *puint16Result)
{
    STD_ReturnType local_Status = E_OK;

    if (puint16Result == NULL)
    {
        local_Status = E_NOK;
    }
    else
    {
        uint8 uint8Low = ADC_ADCL_REG;
        uint8 uint8High = ADC_ADCH_REG;

        *puint16Result = (uint16)(((uint16)uint8High << 8) | uint8Low);
    }

    return local_Status;
}

/*
 * Blocking read: starts a conversion on the given channel, busy-waits until it
 * finishes, then returns the result.
 */
STD_ReturnType ADC_ReadChannelBlocking(uint8 uint8Channel, uint16 *puint16Result)
{
    STD_ReturnType local_Status = ADC_StartConversion(uint8Channel);

    if (local_Status == E_OK)
    {
        while (ADC_IsConversionComplete() == ADC_CONVERSION_BUSY)
        {
            /* wait */
        }

        local_Status = ADC_ReadResult(puint16Result);
    }

    return local_Status;
}
