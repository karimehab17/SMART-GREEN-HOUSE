#ifndef ADC_INTERFACE_H
#define ADC_INTERFACE_H

#include "../../LIB/STD_TYPES.h"


/* =========================================================
 *                    ADC Channel IDs
 * ========================================================= */

#define ADC_CHANNEL_0            0U
#define ADC_CHANNEL_1            1U
#define ADC_CHANNEL_2            2U
#define ADC_CHANNEL_3            3U
#define ADC_CHANNEL_4            4U
#define ADC_CHANNEL_5            5U
#define ADC_CHANNEL_6            6U
#define ADC_CHANNEL_7            7U


/* =========================================================
 *                  ADC Conversion Status
 * ========================================================= */

#define ADC_CONVERSION_DONE      0U
#define ADC_CONVERSION_BUSY      1U


/* =========================================================
 *                    ADC Interrupt State
 * ========================================================= */

#define ADC_INTERRUPT_DISABLE    0U
#define ADC_INTERRUPT_ENABLE     1U


/* =========================================================
 *                         ADC API
 * ========================================================= */

STD_ReturnType ADC_Init(
    uint8 Copy_u8Ref,
    uint8 Copy_u8Prescaler
);

STD_ReturnType ADC_StartConversion(
    uint8 Copy_u8Channel
);

uint8 ADC_IsConversionComplete(void);

STD_ReturnType ADC_ReadResult(
    uint16* Copy_pu16Reading
);

STD_ReturnType ADC_ReadChannelBlocking(
    uint8 Copy_u8Channel,
    uint16* Copy_pu16Reading
);

STD_ReturnType ADC_ReadChannel(
    uint8 Copy_u8Channel,
    uint16* Copy_pu16Reading
);

STD_ReturnType ADC_GetResult(
    uint16* Copy_pu16Reading
);

STD_ReturnType ADC_SetInterrupt(
    uint8 Copy_u8State
);

STD_ReturnType ADC_DeInit(void);


#endif /* ADC_INTERFACE_H */