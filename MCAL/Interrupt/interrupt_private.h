#ifndef INTERRUPT_PRIVATE_H
#define INTERRUPT_PRIVATE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — INTERRUPT private layer (ATmega32)
 * Include this file ONLY from INTERRUPT.c.
 *
 * What you must add here:
 * 1. Registers:
 *      MCUCR   0x55    ISC11 ISC10 ISC01 ISC00
 *      MCUCSR  0x54    ISC2 is bit 6
 *      GICR    0x5B    INT1=7 INT0=6 INT2=5
 *      GIFR    0x5A    INTF1=7 INTF0=6 INTF2=5   (write 1 to clear)
 *      SREG    0x5F    I-bit is bit 7
 *
 * 2. Sense-bit mapping:
 *      INT0 : ISC01 ISC00 in MCUCR
 *      INT1 : ISC11 ISC10 in MCUCR
 *      INT2 : ISC2 in MCUCSR   (0 = falling, 1 = rising) — no low-level / any-change
 *
 * 3. Optional: a small static helper that writes the two ISC bits for INT0/INT1.
 *
 * 4. Do NOT put ISR(INT0_vect) here. The application owns the ISR body.
 *    This driver only arms the source.
 */

/* TODO: map MCUCR, MCUCSR, GICR, GIFR, SREG and the bit names. */
#define MCUCR  (*(volatile uint8 *)0x55u)
#define MCUCSR (*(volatile uint8 *)0x54u)
#define GICR  (*(volatile uint8 *)0x5Bu) // external interrupt control register
#define GIFR  (*(volatile uint8 *)0x5Au)
#define SREG  (*(volatile uint8 *)0x5Fu)
#endif /* INTERRUPT_PRIVATE_H */
