#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

#include "../../LIB/STD_TYPES.h"

/* =========================================================
 *                         TIMER0
 * ========================================================= */

/* Timer0 Modes */

#define TIMER0_NORMAL        0U
#define TIMER0_PHASE_CORRECT 1U
#define TIMER0_CTC           2U
#define TIMER0_FAST_PWM      3U

/* Timer0 Prescaler */

#define TIMER0_STOP          0U
#define TIMER0_PRESC_1       1U
#define TIMER0_PRESC_8       2U
#define TIMER0_PRESC_64      3U
#define TIMER0_PRESC_256     4U
#define TIMER0_PRESC_1024    5U

/* Interrupt State */

#define TIMER_INTERRUPT_DISABLE  0U
#define TIMER_INTERRUPT_ENABLE   1U


STD_ReturnType TIMER0_Init(uint8 Copy_u8Mode);

STD_ReturnType TIMER0_Start(uint8 Copy_u8Prescaler);

STD_ReturnType TIMER0_Stop(void);

STD_ReturnType TIMER0_SetCompareValue(uint8 Copy_u8Value);

STD_ReturnType TIMER0_SetCompareInterrupt(uint8 Copy_u8State);


/* =========================================================
 *                         TIMER2
 * ========================================================= */

STD_ReturnType TIMER2_PWM(uint8 Copy_u8DutyPercent);

STD_ReturnType TIMER2_BuzzerTone(uint16 Copy_u16FrequencyHz);

STD_ReturnType TIMER2_Stop(void);

#endif /* TIMER_INTERFACE_H */