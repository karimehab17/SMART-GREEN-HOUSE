#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — I2C / TWI private layer (ATmega32)
 * Include this file ONLY from I2C.c.
 *
 * What you must add here:
 * 1. Registers:
 *      TWBR  0x20    bit rate
 *      TWSR  0x21    TWS7..TWS3 are status; TWPS1:0 are the prescaler
 *      TWAR  0x22    own slave address (not needed for master-only labs)
 *      TWDR  0x23    address / data
 *      TWCR  0x56    TWINT TWEA TWSTA TWSTO TWWC TWEN – TWIE
 *
 * 2. Bit names in TWCR:
 *      TWINT=7  write 1 to start the next action, poll until hardware sets it
 *      TWEA=6   ACK enable when receiving
 *      TWSTA=5  START
 *      TWSTO=4  STOP
 *      TWEN=2   TWI enable
 *
 * 3. Status mask: (TWSR & 0xF8)  — never compare the raw TWSR (prescaler bits).
 *
 * 4. Every master step is the same pattern:
 *      TWCR = (1<<TWINT) | (1<<TWEN) | extra bits (TWSTA / TWSTO / TWEA)
 *      while (TWINT == 0) ;
 *      if ((TWSR & 0xF8) != expected) return E_NOK;
 *
 * 5. Address byte: (Copy_u8Address << 1) | 0  for write
 *                  (Copy_u8Address << 1) | 1  for read
 *
 * 6. SCL formula (TWPS = 00):
 *      TWBR = ((F_CPU / SCL) - 16) / 2
 *      8 MHz, 100 kHz -> TWBR = 32
 */

/* TODO: map TWBR, TWSR, TWDR, TWCR and the bit names. */

#endif /* I2C_PRIVATE_H */
