#ifndef INTERRUPT_PRIVATE_H
#define INTERRUPT_PRIVATE_H


/* TODO: map MCUCR, MCUCSR, GICR, GIFR, SREG and the bit names. */
#define MCUCR  (*(volatile uint8 *)0x55u)
#define MCUCSR (*(volatile uint8 *)0x54u)
#define GICR  (*(volatile uint8 *)0x5Bu) // external interrupt control register
#define GIFR  (*(volatile uint8 *)0x5Au)
#define SREG  (*(volatile uint8 *)0x5Fu)
#endif /* INTERRUPT_PRIVATE_H */
