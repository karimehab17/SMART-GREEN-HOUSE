/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — ADC.c  (ATmega32, 10-bit)
 * Implement every prototype from ADC_interface.h.
 */

#include "STD_TYPES.h"
#include "ADC_interface.h"
#include "ADC_private.h"
#include "stddef.h"
#include "util/atomic.h"
#include "util/delay.h"


/*
 * ADC_Init
 * 1. Reject an unknown reference or prescaler.
 * 2. Write REFS1:0 (and ADLAR = 0 for right adjust) in ADMUX.
 * 3. Write ADPS2:0, then set ADEN. Do not start a conversion yet.
 * 4. Target ADC clock 50..200 kHz (8 MHz / 64 = 125 kHz).
 */
STD_ReturnType ADC_Init(uint8 Copy_u8Ref, uint8 Copy_u8Prescaler)
{
    if (Copy_u8Ref != ADC_REF_AREF &&
        Copy_u8Ref != ADC_REF_AVCC &&
        Copy_u8Ref != ADC_REF_INTERNAL_2V56)
    {
        return E_NOK;
    }

    if (Copy_u8Prescaler != ADC_PRESC_64 &&
        Copy_u8Prescaler != ADC_PRESC_128)
    {
        return E_NOK;
    }

    ADMUX = (ADMUX & ~(0xC0)) |
            ((Copy_u8Ref << 6) & 0xC0);

    ADMUX &= ~(1 << ADLAR);

    ADCSRA = (ADCSRA & ~(0x07)) |
             (Copy_u8Prescaler & 0x07);

    ADCSRA |= (1 << ADEN);

    return E_OK;
}
/*
 * ADC_ReadChannel
 * 1. Reject Channel > 7 or a NULL pointer.
 * 2. Keep REFS bits, replace MUX4:0 with the channel.
 * 3. Set ADSC. Poll ADIF (or ADSC) until the conversion ends.
 * 4. Clear ADIF by writing 1 to it.
 * 5. Read ADCL then ADCH. Combine: reading = ADCL | ((uint16)ADCH << 8).
 */
STD_ReturnType ADC_ReadChannel(uint8 Copy_u8Channel, uint16 *Copy_pu16Reading)
{
    if (Copy_u8Channel > ADC_CHANNEL_7 || Copy_pu16Reading == NULL)
    {
        return E_NOK;
    }

    ADMUX = (ADMUX & ~(0x1F)) | (Copy_u8Channel & 0x1F);
    ADCSRA |= (1 << ADSC);

    while (ADCSRA & (1 << ADSC))
    {
    }

    ADCSRA |= (1 << ADIF);
    *Copy_pu16Reading = ADCL;
    *Copy_pu16Reading |= ((uint16)ADCH << 8);

    return E_OK;
}

/*
 * ADC_StartConversion
 * 1. Select the channel as above.
 * 2. Set ADSC and return. Used when the result will be read later or in an ISR.
 */
STD_ReturnType ADC_StartConversion(uint8 Copy_u8Channel)
{
    if (Copy_u8Channel > ADC_CHANNEL_7)
    {
        return E_NOK;
    }

    ADMUX = (ADMUX & ~(0x1F)) | (Copy_u8Channel & 0x1F);
    ADCSRA |= (1 << ADSC);

    return E_OK;
}

/*
 * ADC_GetResult
 * 1. If ADIF is 0, return E_NOK (still busy).
 * 2. Clear ADIF, read ADCL then ADCH, store the 10-bit value.
 */
STD_ReturnType ADC_GetResult(uint16 *Copy_pu16Reading)
{
    if (Copy_pu16Reading == NULL)
    {
        return E_NOK;
    }

    if ((ADCSRA & (1 << ADIF)) == 0)
    {
        return E_NOK;
    }

    ADCSRA |= (1 << ADIF);
    *Copy_pu16Reading = ADCL;
    *Copy_pu16Reading |= ((uint16)ADCH << 8);

    return E_OK;
}

/*
 * ADC_SetInterrupt
 * 1. Copy_u8State == 1 -> set ADIE.  == 0 -> clear ADIE.
 * 2. The ISR vector is ADC_vect. Do not write the ISR in this file unless asked.
 */