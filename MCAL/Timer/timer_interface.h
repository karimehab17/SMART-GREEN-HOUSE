#ifndef TIMER_INTERFACE_H
#define TIMER_INTERFACE_H

#include "../../LIB/STD_TYPES.h"

/*========================== Timer0 Modes ==========================*/

#define TIMER0_NORMAL          0u
#define TIMER0_PHASE_CORRECT  1u
#define TIMER0_CTC            2u
#define TIMER0_FAST_PWM       3u

/* Timer0 Clock Select / Prescaler */
#define TIMER0_STOP            0u
#define TIMER0_PRESC_1         1u
#define TIMER0_PRESC_8         2u
#define TIMER0_PRESC_64        3u
#define TIMER0_PRESC_256       4u
#define TIMER0_PRESC_1024      5u
#define TIMER0_EXT_FALLING     6u
#define TIMER0_EXT_RISING      7u

/* Timer0 Compare Output Mode */
#define TIMER0_OC_DISCONNECT   0u
#define TIMER0_OC_TOGGLE       1u
#define TIMER0_OC_NON_INVERT   2u
#define TIMER0_OC_INVERT       3u


/*
 * Description : Prepare Timer0 for the delay functions (1 ms tick).
 *               Call it once before TIMER0_DelayMS or TIMER0_DelayS.
 */
STD_ReturnType TIMER0_Init(uint8 Copy_u8Mode);

/*
 * Description : Block for Copy_u16Milliseconds (1 .. 65535 ms), then return.
 */
STD_ReturnType TIMER0_DelayMS(uint16 Copy_u16Milliseconds);

/*
 * Description : Block for Copy_u16Seconds seconds, then return.
 */
STD_ReturnType TIMER0_DelayS(uint16 Copy_u16Seconds);

/*
 * Description : Output a ~490 Hz PWM wave on OC0 (PB3) and leave it running.
 *               Configures the timer itself, so TIMER0_Init is not needed first.
 * Parameter   : Copy_u8DutyPercent — 0..100.
 * Return      : E_OK, or E_NOK above 100.
 */
STD_ReturnType TIMER0_PWM(uint8 Copy_u8DutyPercent);

/*
 * Description : Stop Timer0 and release OC0 (PB3) back to plain GPIO.
 */
STD_ReturnType TIMER0_Stop(void);

STD_ReturnType TIMER0_Start(uint8 Copy_u8Prescaler);
STD_ReturnType TIMER0_SetCompareValue(uint8 Copy_u8Value);
STD_ReturnType TIMER0_SetCompareOutput(uint8 Copy_u8Mode);
STD_ReturnType TIMER0_SetOverflowInterrupt(uint8 Copy_u8State);
STD_ReturnType TIMER0_SetCompareInterrupt(uint8 Copy_u8State);

/*========================== Timer1 — 16-bit ==========================*/

/*
 * Description : Prepare Timer1 for the delay functions (1 ms tick).
 */
STD_ReturnType TIMER1_Init(uint8 Copy_u8Mode);

/*
 * Description : Block for Copy_u16Milliseconds (1 .. 65535 ms), then return.
 */
STD_ReturnType TIMER1_DelayMS(uint16 Copy_u16Milliseconds);

/*
 * Description : Output PWM on OC1A (PD5) at a frequency you choose, and leave
 *               it running. Configures the timer itself.
 * Parameters  : Copy_u16FrequencyHz — 16 .. 20000 Hz. Servos use 50.
 *               Copy_u8DutyPercent  — 0..100.
 * Return      : E_OK, or E_NOK for a frequency out of range or duty above 100.
 */
STD_ReturnType TIMER1_PWM(uint16 Copy_u16FrequencyHz, uint8 Copy_u8DutyPercent);

/*
 * Description : Stop Timer1 and release OC1A (PD5) back to plain GPIO.
 */
STD_ReturnType TIMER1_Stop(void);


/*========================== Timer2 — 8-bit PWM ==========================*/

/*
 * Fast PWM output on OC2 (PD7).
 * Duty cycle: 0..100%
 */
STD_ReturnType TIMER2_PWM(uint8 Copy_u8DutyPercent);

/*
 * Generate a buzzer tone using Timer2 Fast PWM.
 * Supported frequencies are selected using the closest Timer2 prescaler.
 */
STD_ReturnType TIMER2_BuzzerTone(uint16 Copy_u16FrequencyHz);

/*
 * Stop Timer2 PWM and disconnect OC2.
 */
STD_ReturnType TIMER2_Stop(void);
    
#endif /* TIMER_INTERFACE_H */
