#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * MCAL ADC — public API for the ATmega32 10-bit ADC (channels ADC0..ADC7).
 * Include this header from HAL, Logic, and main. Do not include ADC_private.h there.
 */

/* ---------------- Voltage reference (ADMUX REFS1:0) ---------------- */
#define ADC_REF_AREF          0u
#define ADC_REF_AVCC          1u
#define ADC_REF_INTERNAL_2V56 3u

/* ---------------- Result adjust (ADMUX ADLAR) ---------------- */
#define ADC_RIGHT_ADJUST      0u
#define ADC_LEFT_ADJUST       1u

/* ---------------- Prescaler (ADCSRA ADPS2:0) ---------------- */
#define ADC_PRESC_2           1u
#define ADC_PRESC_4           2u
#define ADC_PRESC_8           3u
#define ADC_PRESC_16          4u
#define ADC_PRESC_32          5u
#define ADC_PRESC_64          6u
#define ADC_PRESC_128         7u

/* ---------------- Single-ended channels ---------------- */
#define ADC_CHANNEL_0         0u
#define ADC_CHANNEL_1         1u
#define ADC_CHANNEL_2         2u
#define ADC_CHANNEL_3         3u
#define ADC_CHANNEL_4         4u
#define ADC_CHANNEL_5         5u
#define ADC_CHANNEL_6         6u
#define ADC_CHANNEL_7         7u

/* ---------------- Conversion status ---------------- */
#define ADC_CONVERSION_DONE  0u
#define ADC_CONVERSION_BUSY  1u

/* ---------------- API ---------------- */

STD_ReturnType ADC_Init(uint8 Copy_u8Ref,
                        uint8 Copy_u8Prescaler);

STD_ReturnType ADC_ReadChannel(uint8 Copy_u8Channel,
                               uint16 *Copy_pu16Reading);

STD_ReturnType ADC_StartConversion(uint8 Copy_u8Channel);

uint8 ADC_IsConversionComplete(void);

STD_ReturnType ADC_ReadResult(uint16 *Copy_pu16Reading);

STD_ReturnType ADC_ReadChannelBlocking(uint8 Copy_u8Channel,
                                       uint16 *Copy_pu16Reading);

STD_ReturnType ADC_GetResult(uint16 *Copy_pu16Reading);

STD_ReturnType ADC_SetInterrupt(uint8 Copy_u8State);

STD_ReturnType ADC_DeInit(void);

#endif /* ADC_INTERFACE_H */
