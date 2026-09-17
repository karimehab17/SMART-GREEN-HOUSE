#ifndef ADC_PRIVATE_H
#define ADC_PRIVATE_H

#include "../../LIB/STD_TYPES.h"


/* =========================================================
 *                     ADC Registers
 * ========================================================= */

#define ADC_ADMUX_REG      (*(volatile uint8*)0x27U)
#define ADC_ADCSRA_REG     (*(volatile uint8*)0x26U)
#define ADC_ADCH_REG       (*(volatile uint8*)0x25U)
#define ADC_ADCL_REG       (*(volatile uint8*)0x24U)
#define ADC_SFIOR_REG      (*(volatile uint8*)0x50U)


/* =========================================================
 *                       ADMUX Bits
 * ========================================================= */

#define ADC_MUX0_BIT       0U
#define ADC_MUX1_BIT       1U
#define ADC_MUX2_BIT       2U
#define ADC_MUX3_BIT       3U
#define ADC_MUX4_BIT       4U

#define ADC_ADLAR_BIT      5U

#define ADC_REFS0_BIT      6U
#define ADC_REFS1_BIT      7U


/* =========================================================
 *                      ADCSRA Bits
 * ========================================================= */

#define ADC_ADPS0_BIT      0U
#define ADC_ADPS1_BIT      1U
#define ADC_ADPS2_BIT      2U

#define ADC_ADIE_BIT       3U
#define ADC_ADIF_BIT       4U
#define ADC_ADATE_BIT      5U
#define ADC_ADSC_BIT       6U
#define ADC_ADEN_BIT       7U


/* =========================================================
 *                       SFIOR Bits
 * ========================================================= */

#define ADC_ADTS0_BIT      5U
#define ADC_ADTS1_BIT      6U
#define ADC_ADTS2_BIT      7U


/* =========================================================
 *                  Internal ADC Constants
 * ========================================================= */

#define ADC_NUMBER_OF_CHANNELS    8U


#endif /* ADC_PRIVATE_H */