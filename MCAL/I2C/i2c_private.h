#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

/*
 * I2C / TWI private layer
 * ATmega32
 *
 * Include this file ONLY from I2C.c.
 */

/* ================= TWI Registers ================= */

#define TWBR    (*(volatile uint8 *)0x20u)
#define TWSR    (*(volatile uint8 *)0x21u)
#define TWAR    (*(volatile uint8 *)0x22u)
#define TWDR    (*(volatile uint8 *)0x23u)
#define TWCR    (*(volatile uint8 *)0x56u)

/* ================= TWCR Bit Positions ================= */

#define TWINT   7u
#define TWEA    6u
#define TWSTA   5u
#define TWSTO   4u
#define TWWC    3u
#define TWEN    2u
#define TWIE    0u

/* ================= TWSR ================= */

/*
 * TWS7..TWS3 = Status bits
 * TWPS1..TWPS0 = Prescaler bits
 *
 * Always mask status using:
 * (TWSR & 0xF8)
 */

#define TWS_STATUS_MASK     0xF8u

/* TWI prescaler = 1 */
#define TWI_PRESCALER_1    0u

#endif /* I2C_PRIVATE_H */