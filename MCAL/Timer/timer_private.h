#ifndef TIMER_PRIVATE_H
#define TIMER_PRIVATE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — TIMER private layer (ATmega32)
 * Include this file ONLY from TIMER.c.
 *
 * What you must add here:
 * 1. Timer0 registers (I/O space):
 *      TCCR0  0x53    FOC0 WGM00 COM01 COM00 WGM01 CS02 CS01 CS00
 *      TCNT0  0x52
 *      OCR0   0x5C
 * 2. Timer1 registers:
 *      TCCR1A 0x4F    COM1A1 COM1A0 COM1B1 COM1B0 FOC1A FOC1B WGM11 WGM10
 *      TCCR1B 0x4E    ICNC1  ICES1  – WGM13 WGM12 CS12 CS11 CS10
 *      TCNT1  0x4C    (16-bit, write high byte first)
 *      OCR1A  0x4A
 *      ICR1   0x46
 * 3. Shared:
 *      TIMSK  0x59    OCIE2 TOIE2 TICIE1 OCIE1A OCIE1B TOIE1 OCIE0 TOIE0
 *      TIFR   0x58    matching flags — write 1 to clear
 *
 * 4. Bit-position macros for WGM, CS, COM, TOIE0, OCIE0, OCF0, TOV0.
 *
 * 5. Remember: a flag is cleared by writing 1 to it (w1c).
 */
#define TIMER0_REG_TCCR0 (*(volatile uint8 *)0x53)
#define TIMER0_REG_TCNT0 (*(volatile uint8 *)0x52)
#define TIMER0_REG_OCR0 (*(volatile uint8 *)0x5C)

#define TIMER1_REG_TCCR1A (*(volatile uint8 *)0x4F)
#define TIMER1_REG_TCCR1B (*(volatile uint8 *)0x4E)
#define TIMER1_REG_TCNT1 (*(volatile uint16 *)0x4C)
#define TIMER1_REG_OCR1A (*(volatile uint16 *)0x4A)
#define TIMER1_REG_ICR1 (*(volatile uint16 *)0x46)

#define TIMSK_REG (*(volatile uint8 *)0x59)
#define TIFR_REG (*(volatile uint8 *)0x58)

/* Timer0 bit positions */
#define FOC0 7
#define WGM00 6
#define COM01 5
#define COM00 4
#define WGM01 3
#define CS02 2
#define CS01 1
#define CS00 0
#define TOIE0 0
#define OCIE0 1
#define OCF0 1
#define TOV0 0

/* Timer1 bit positions */
#define COM1A1 7
#define COM1A0 6
#define COM1B1 5
#define COM1B0 4
#define FOC1A 3
#define FOC1B 2
#define WGM11 1
#define WGM10 0
#define ICNC1 7
#define ICES1 6
#define WGM13 4
#define WGM12 3
#define CS12 2
#define CS11 1
#define CS10 0
#define TOV1 2

/* Timer2 registers */
#define TIMER2_REG_TCCR2 (*(volatile uint8 *)0x45)
#define TIMER2_REG_TCNT2 (*(volatile uint8 *)0x44)
#define TIMER2_REG_OCR2  (*(volatile uint8 *)0x43)

/* Timer2 bit positions */
#define FOC2  7
#define WGM20 6
#define COM21 5
#define COM20 4
#define WGM21 3
#define CS22  2
#define CS21  1
#define CS20  0

/* Timer2 interrupt bits */
#define OCIE2 7
#define TOIE2 6

#endif /* TIMER_PRIVATE_H */
