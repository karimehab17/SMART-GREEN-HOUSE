#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * MCAL ADC — public API for the ATmega32 10-bit ADC (channels ADC0..ADC7).
 * Include this header from HAL, Logic, and main. Do not include ADC_private.h there.
 */

#include "STD_TYPES.h"

/* ---------------- Voltage reference (ADMUX REFS1:0) ---------------- */
#define ADC_REF_AREF          0u    /* AREF pin, internal Vref off */
#define ADC_REF_AVCC          1u    /* AVCC with cap on AREF      */
#define ADC_REF_INTERNAL_2V56 3u    /* Internal 2.56 V            */

/* ---------------- Result adjust (ADMUX ADLAR) ---------------- */
#define ADC_RIGHT_ADJUST      0u    /* 10-bit value in ADC = ADCL | (ADCH << 8) */
#define ADC_LEFT_ADJUST       1u

/* ---------------- Prescaler (ADCSRA ADPS2:0) — F_ADC = F_CPU / N ---------------- */
#define ADC_PRESC_2           1u
#define ADC_PRESC_4           2u
#define ADC_PRESC_8           3u
#define ADC_PRESC_16          4u
#define ADC_PRESC_32          5u
#define ADC_PRESC_64          6u
#define ADC_PRESC_128         7u

/* ---------------- Single-ended channels (ADMUX MUX4:0) ---------------- */
#define ADC_CHANNEL_0         0u
#define ADC_CHANNEL_1         1u
#define ADC_CHANNEL_2         2u
#define ADC_CHANNEL_3         3u
#define ADC_CHANNEL_4         4u
#define ADC_CHANNEL_5         5u
#define ADC_CHANNEL_6         6u
#define ADC_CHANNEL_7         7u

/*
 * Description : Enable the ADC, pick the reference and the prescaler.
 *               Typical kit: ADC_REF_AVCC and ADC_PRESC_64 at 8 MHz (~125 kHz).
 */
STD_ReturnType ADC_Init(uint8 Copy_u8Ref, uint8 Copy_u8Prescaler);
/*
 * Description : Select the channel (0..7), start one conversion, wait for ADIF,
 *               then write the 10-bit result to *Copy_pu16Reading (0..1023).
 */
STD_ReturnType ADC_ReadChannel(uint8 Copy_u8Channel, uint16 *Copy_pu16Reading);

/*
 * Description : Start a conversion on a channel already selected; do not wait.
 */
STD_ReturnType ADC_StartConversion(uint8 Copy_u8Channel);

/*
 * Description : Return E_OK and the last 10-bit result if ADIF is set.
 *               Return E_NOK if the conversion is still running.
 */
STD_ReturnType ADC_GetResult(uint16 *Copy_pu16Reading);

/*
 * Description : Enable or disable the ADC complete interrupt (ADIE).
 *               Copy_u8State: 1 = enable, 0 = disable. Call sei() from INTERRUPT.
 */
STD_ReturnType ADC_SetInterrupt(uint8 Copy_u8State);

#endif /* ADC_INTERFACE_H */
