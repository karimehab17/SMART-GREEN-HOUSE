#ifndef TIMER_PRIVATE_H
#define TIMER_PRIVATE_H

#include "../../LIB/STD_TYPES.h"

/* ==================== Timer0 Registers ==================== */

#define TIMER0_TCCR0_REG    (*(volatile uint8*)0x53U)
#define TIMER0_TCNT0_REG    (*(volatile uint8*)0x52U)
#define TIMER0_OCR0_REG     (*(volatile uint8*)0x5CU)

/* ==================== Timer2 Registers ==================== */

#define TIMER2_TCCR2_REG    (*(volatile uint8*)0x45U)
#define TIMER2_TCNT2_REG    (*(volatile uint8*)0x44U)
#define TIMER2_OCR2_REG     (*(volatile uint8*)0x43U)

/* ==================== Shared Registers ==================== */

#define TIMER_TIMSK_REG     (*(volatile uint8*)0x59U)
#define TIMER_TIFR_REG      (*(volatile uint8*)0x58U)

/* ==================== Timer0 Bits ==================== */

#define TIMER0_FOC0_BIT     7U
#define TIMER0_WGM00_BIT    6U
#define TIMER0_COM01_BIT    5U
#define TIMER0_COM00_BIT    4U
#define TIMER0_WGM01_BIT    3U
#define TIMER0_CS02_BIT     2U
#define TIMER0_CS01_BIT     1U
#define TIMER0_CS00_BIT     0U

/* ==================== Timer0 Interrupt Bits ==================== */

#define TIMER0_TOIE0_BIT    0U
#define TIMER0_OCIE0_BIT    1U
#define TIMER0_TOV0_BIT     0U
#define TIMER0_OCF0_BIT     1U

/* ==================== Timer2 Bits ==================== */

#define TIMER2_FOC2_BIT     7U
#define TIMER2_WGM20_BIT    6U
#define TIMER2_COM21_BIT    5U
#define TIMER2_COM20_BIT    4U
#define TIMER2_WGM21_BIT    3U
#define TIMER2_CS22_BIT     2U
#define TIMER2_CS21_BIT     1U
#define TIMER2_CS20_BIT     0U

/* ==================== Timer2 Interrupt Bits ==================== */

#define TIMER2_OCIE2_BIT    7U
#define TIMER2_TOIE2_BIT    6U

#endif /* TIMER_PRIVATE_H */