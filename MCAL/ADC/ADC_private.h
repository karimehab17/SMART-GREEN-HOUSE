#ifndef ADC_PRIVATE_H
#define ADC_PRIVATE_H

/* ---------------- ADC Registers (ATmega32 memory-mapped addresses) ---------------- */
#define ADC_ADMUX_REG      (*(volatile uint8  *)0x27)
#define ADC_ADCSRA_REG     (*(volatile uint8  *)0x26)
#define ADC_ADCH_REG       (*(volatile uint8  *)0x25)
#define ADC_ADCL_REG       (*(volatile uint8  *)0x24)
#define ADC_ADCDATA_REG    (*(volatile uint16 *)0x24)
#define ADC_SFIOR_REG      (*(volatile uint8  *)0x50)

/* ---------------- ADMUX bit positions ---------------- */
#define ADC_MUX0_BIT       0
#define ADC_MUX1_BIT       1
#define ADC_MUX2_BIT       2
#define ADC_MUX3_BIT       3
#define ADC_MUX4_BIT       4
#define ADC_ADLAR_BIT      5
#define ADC_REFS0_BIT      6
#define ADC_REFS1_BIT      7

/* ---------------- ADCSRA bit positions ---------------- */
#define ADC_ADPS0_BIT      0
#define ADC_ADPS1_BIT      1
#define ADC_ADPS2_BIT      2
#define ADC_ADATE_BIT      5
#define ADC_ADSC_BIT       6
#define ADC_ADEN_BIT       7

/* ---------------- SFIOR bit positions ---------------- */
#define ADC_ADTS0_BIT      5
#define ADC_ADTS1_BIT      6
#define ADC_ADTS2_BIT      7

#define ADC_NUMBER_OF_CHANNELS  8

#endif