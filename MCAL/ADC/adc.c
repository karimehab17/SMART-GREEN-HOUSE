#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"

#include "ADC_interface.h"
#include "ADC_private.h"

#include <stddef.h>


/* =========================================================
 *                     Private Helpers
 * ========================================================= */

static STD_ReturnType ADC_IsValidChannel(uint8 Copy_u8Channel)
{
    if (Copy_u8Channel < ADC_NUMBER_OF_CHANNELS)
    {
        return E_OK;
    }

    return E_NOK;
}


static uint8 ADC_IsBusy(void)
{
    return GET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT);
}


static void ADC_SelectChannel(uint8 Copy_u8Channel)
{
    ADC_ADMUX_REG &= 0xE0U;
    ADC_ADMUX_REG |= (Copy_u8Channel & 0x1FU);
}


static void ADC_SetPrescaler(uint8 Copy_u8Prescaler)
{
    ADC_ADCSRA_REG &= 0xF8U;
    ADC_ADCSRA_REG |= (Copy_u8Prescaler & 0x07U);
}


static void ADC_SetReference(uint8 Copy_u8Ref)
{
    ADC_ADMUX_REG &= 0x3FU;
    ADC_ADMUX_REG |= (uint8)((Copy_u8Ref & 0x03U) << 6U);
}


/* =========================================================
 *                         Initialization
 * ========================================================= */

STD_ReturnType ADC_Init(
    uint8 Copy_u8Ref,
    uint8 Copy_u8Prescaler)
{
    if (Copy_u8Ref > ADC_REF_INTERNAL_2V56)
    {
        return E_NOK;
    }

    if (Copy_u8Prescaler < ADC_PRESC_2 ||
        Copy_u8Prescaler > ADC_PRESC_128)
    {
        return E_NOK;
    }

    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    ADC_SetReference(Copy_u8Ref);

    CLEAR_BIT(ADC_ADMUX_REG, ADC_ADLAR_BIT);

    ADC_SelectChannel(ADC_CHANNEL_0);

    ADC_SetPrescaler(Copy_u8Prescaler);

    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADATE_BIT);

    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADIE_BIT);

    SET_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    return E_OK;
}


/* =========================================================
 *                    Start Conversion
 * ========================================================= */

STD_ReturnType ADC_StartConversion(uint8 Copy_u8Channel)
{
    if (ADC_IsValidChannel(Copy_u8Channel) != E_OK)
    {
        return E_NOK;
    }

    /*
     * ADMUX may only be changed while no conversion
     * is currently running.
     */
    if (ADC_IsBusy() != 0U)
    {
        return E_NOK;
    }

    ADC_SelectChannel(Copy_u8Channel);

    SET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT);

    return E_OK;
}


/* =========================================================
 *                 Conversion Status
 * ========================================================= */

uint8 ADC_IsConversionComplete(void)
{
    if (ADC_IsBusy() != 0U)
    {
        return ADC_CONVERSION_BUSY;
    }

    return ADC_CONVERSION_DONE;
}


/* =========================================================
 *                      Read Result
 * ========================================================= */

STD_ReturnType ADC_ReadResult(uint16* Copy_pu16Reading)
{
    uint8 Local_u8Low;
    uint8 Local_u8High;

    if (Copy_pu16Reading == NULL)
    {
        return E_NOK;
    }

    if (ADC_IsBusy() != 0U)
    {
        return E_NOK;
    }

    /*
     * ADCL must be read before ADCH.
     */
    Local_u8Low = ADC_ADCL_REG;
    Local_u8High = ADC_ADCH_REG;

    *Copy_pu16Reading =
        (uint16)(((uint16)Local_u8High << 8U) | Local_u8Low);

    *Copy_pu16Reading &= 0x03FFU;

    return E_OK;
}


/* =========================================================
 *                  Read Channel
 * ========================================================= */

STD_ReturnType ADC_ReadChannel(
    uint8 Copy_u8Channel,
    uint16* Copy_pu16Reading)
{
    return ADC_ReadChannelBlocking(
        Copy_u8Channel,
        Copy_pu16Reading
    );
}


/* =========================================================
 *                Blocking Channel Read
 * ========================================================= */

STD_ReturnType ADC_ReadChannelBlocking(
    uint8 Copy_u8Channel,
    uint16* Copy_pu16Reading)
{
    STD_ReturnType Local_u8Status;

    if (Copy_pu16Reading == NULL)
    {
        return E_NOK;
    }

    Local_u8Status = ADC_StartConversion(Copy_u8Channel);

    if (Local_u8Status != E_OK)
    {
        return Local_u8Status;
    }

    while (ADC_IsConversionComplete() == ADC_CONVERSION_BUSY)
    {
        /* Wait for conversion completion */
    }

    return ADC_ReadResult(Copy_pu16Reading);
}


/* =========================================================
 *                     Get Result
 * ========================================================= */

STD_ReturnType ADC_GetResult(uint16* Copy_pu16Reading)
{
    return ADC_ReadResult(Copy_pu16Reading);
}


/* =========================================================
 *                  ADC Interrupt Control
 * ========================================================= */

STD_ReturnType ADC_SetInterrupt(uint8 Copy_u8State)
{
    if (Copy_u8State > ADC_INTERRUPT_ENABLE)
    {
        return E_NOK;
    }

    if (Copy_u8State == ADC_INTERRUPT_ENABLE)
    {
        SET_BIT(ADC_ADCSRA_REG, ADC_ADIE_BIT);
    }
    else
    {
        CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADIE_BIT);
    }

    return E_OK;
}


/* =========================================================
 *                        De-Init
 * ========================================================= */

STD_ReturnType ADC_DeInit(void)
{
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADIE_BIT);

    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADATE_BIT);

    return E_OK;
}