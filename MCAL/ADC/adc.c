#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"

#include "../../src/config.h"

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
    /*
     * Preserve REFS1:0 and ADLAR.
     * Modify only MUX4:0.
     */
    ADC_ADMUX_REG &= 0xE0U;
    ADC_ADMUX_REG |= (Copy_u8Channel & 0x1FU);
}


static void ADC_SetPrescaler(uint8 Copy_u8Prescaler)
{
    /*
     * ADPS2:0 occupy bits 2:0.
     */
    ADC_ADCSRA_REG &= 0xF8U;
    ADC_ADCSRA_REG |= (Copy_u8Prescaler & 0x07U);
}


static void ADC_SetReference(uint8 Copy_u8Ref)
{
    /*
     * REFS1:0 occupy bits 7:6.
     */
    ADC_ADMUX_REG &= 0x3FU;
    ADC_ADMUX_REG |=
        (uint8)((Copy_u8Ref & 0x03U) << 6U);
}


/* =========================================================
 *                     Initialization
 * ========================================================= */

STD_ReturnType ADC_Init(
    uint8 Copy_u8Ref,
    uint8 Copy_u8Prescaler)
{
    /*
     * The project configuration is defined in config.h.
     *
     * The ADC driver accepts the selected configuration
     * through its initialization API and does not duplicate
     * project-specific values here.
     */

    /*
     * Validate voltage reference.
     *
     * ATmega32A:
     *     0 -> AREF
     *     1 -> AVCC
     *     2 -> Reserved
     *     3 -> Internal 2.56V
     */
    if (Copy_u8Ref == 2U)
    {
        return E_NOK;
    }

    /*
     * Validate the 3-bit ADC prescaler field.
     *
     * Values 1..7 are valid ADC prescaler selections.
     */
    if ((Copy_u8Prescaler < 1U) ||
        (Copy_u8Prescaler > 7U))
    {
        return E_NOK;
    }

    /*
     * Disable ADC before configuration.
     */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    /*
     * Configure voltage reference.
     */
    ADC_SetReference(Copy_u8Ref);

    /*
     * Configure right-adjusted ADC result.
     */
    CLEAR_BIT(ADC_ADMUX_REG, ADC_ADLAR_BIT);

    /*
     * Start from ADC channel 0.
     */
    ADC_SelectChannel(ADC_CHANNEL_0);

    /*
     * Configure ADC clock prescaler.
     */
    ADC_SetPrescaler(Copy_u8Prescaler);

    /*
     * Use single-conversion mode.
     */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADATE_BIT);

    /*
     * Disable ADC interrupt by default.
     */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADIE_BIT);

    /*
     * Clear pending ADC interrupt flag.
     */
    SET_BIT(ADC_ADCSRA_REG, ADC_ADIF_BIT);

    /*
     * Enable ADC.
     */
    SET_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    return E_OK;
}


/* =========================================================
 *                  Start Conversion
 * ========================================================= */

STD_ReturnType ADC_StartConversion(uint8 Copy_u8Channel)
{
    /*
     * Validate ADC channel.
     */
    if (ADC_IsValidChannel(Copy_u8Channel) != E_OK)
    {
        return E_NOK;
    }

    /*
     * Do not change the channel while a conversion is active.
     */
    if (ADC_IsBusy() != 0U)
    {
        return E_NOK;
    }

    /*
     * Select requested channel.
     */
    ADC_SelectChannel(Copy_u8Channel);

    /*
     * Clear previous conversion-complete flag.
     */
    SET_BIT(ADC_ADCSRA_REG, ADC_ADIF_BIT);

    /*
     * Start conversion.
     */
    SET_BIT(ADC_ADCSRA_REG, ADC_ADSC_BIT);

    return E_OK;
}


/* =========================================================
 *                  Conversion Status
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
 *                     Read Result
 * ========================================================= */

STD_ReturnType ADC_ReadResult(uint16* Copy_pu16Reading)
{
    uint8 Local_u8Low;
    uint8 Local_u8High;

    /*
     * Validate destination pointer.
     */
    if (Copy_pu16Reading == NULL)
    {
        return E_NOK;
    }

    /*
     * Do not read ADC registers while conversion is active.
     */
    if (ADC_IsBusy() != 0U)
    {
        return E_NOK;
    }

    /*
     * ADCL must be read before ADCH.
     */
    Local_u8Low = ADC_ADCL_REG;
    Local_u8High = ADC_ADCH_REG;

    /*
     * Combine the 10-bit ADC result.
     */
    *Copy_pu16Reading =
        (uint16)(((uint16)Local_u8High << 8U) |
                 Local_u8Low);

    /*
     * Keep only the 10 valid ADC bits.
     */
    *Copy_pu16Reading &= 0x03FFU;

    return E_OK;
}


/* =========================================================
 *                Blocking Channel Read
 * ========================================================= */

STD_ReturnType ADC_ReadChannelBlocking(
    uint8 Copy_u8Channel,
    uint16* Copy_pu16Reading)
{
    STD_ReturnType Local_u8Status;

    /*
     * Validate destination pointer.
     */
    if (Copy_pu16Reading == NULL)
    {
        return E_NOK;
    }

    /*
     * Start conversion.
     */
    Local_u8Status =
        ADC_StartConversion(Copy_u8Channel);

    if (Local_u8Status != E_OK)
    {
        return Local_u8Status;
    }

    /*
     * Wait for conversion completion.
     *
     * This function is intentionally blocking.
     * The scheduled application should use the
     * non-blocking ADC API when timing matters.
     */
    while (ADC_IsConversionComplete() == ADC_CONVERSION_BUSY)
    {
        /* Wait for conversion completion. */
    }

    /*
     * Read completed conversion.
     */
    return ADC_ReadResult(Copy_pu16Reading);
}


/* =========================================================
 *                     Read Channel
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
 *                      Get Result
 * ========================================================= */

STD_ReturnType ADC_GetResult(uint16* Copy_pu16Reading)
{
    return ADC_ReadResult(Copy_pu16Reading);
}


/* =========================================================
 *                   Interrupt Control
 * ========================================================= */

STD_ReturnType ADC_SetInterrupt(uint8 Copy_u8State)
{
    /*
     * Only 0 and 1 are valid states.
     */
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
 *                         De-Init
 * ========================================================= */

STD_ReturnType ADC_DeInit(void)
{
    /*
     * Disable ADC peripheral.
     */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADEN_BIT);

    /*
     * Disable ADC interrupt.
     */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADIE_BIT);

    /*
     * Disable auto-triggering.
     */
    CLEAR_BIT(ADC_ADCSRA_REG, ADC_ADATE_BIT);

    return E_OK;
}