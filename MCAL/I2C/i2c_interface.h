#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * MCAL I2C — public API for the ATmega32 TWI master.
 * Include this header from HAL, Logic, and main. Do not include I2C_private.h there.
 *
 * Pins: SCL = PC0, SDA = PC1 (open-drain, external pull-ups required).
 */

#include "STD_TYPES.h"

/* ---------------- Status codes (TWSR & 0xF8) ---------------- */
#define I2C_START_ACK         0x08u
#define I2C_REP_START_ACK     0x10u
#define I2C_SLA_W_ACK         0x18u
#define I2C_SLA_R_ACK         0x40u
#define I2C_DATA_TX_ACK       0x28u
#define I2C_DATA_RX_ACK       0x50u
#define I2C_DATA_RX_NACK      0x58u

#define I2C_ACK               1u
#define I2C_NACK              0u

/*
 * Description : Enable TWI as master. TWBR = ((F_CPU / SCL) - 16) / 2  (prescaler = 1).
 *               Typical EEPROM: 100000UL.
 */
STD_ReturnType I2C_InitMaster(uint32 Copy_u32SclHz);

/*
 * Description : Send START. Return E_OK if TWSR status is I2C_START_ACK.
 */
STD_ReturnType I2C_SendStart(void);

/*
 * Description : Send a repeated START. Expect I2C_REP_START_ACK.
 */
STD_ReturnType I2C_SendRepeatedStart(void);

/*
 * Description : Send STOP. Does not wait for a status code.
 */
void I2C_SendStop(void);

/*
 * Description : Address is the 7-bit value, unshifted — the driver shifts it
 *               and adds the R/W bit (0 = write, 1 = read).
 */
STD_ReturnType I2C_SendSlaveAddressWithWrite(uint8 Copy_u8Address);
STD_ReturnType I2C_SendSlaveAddressWithRead(uint8 Copy_u8Address);

/*
 * Description : Write one data byte. Expect I2C_DATA_TX_ACK.
 */
STD_ReturnType I2C_SendByte(uint8 Copy_u8Data);

/*
 * Description : Read one byte. Copy_u8SendAck = I2C_ACK (more bytes coming)
 *               or I2C_NACK (last byte).
 */
STD_ReturnType I2C_ReceiveByte(uint8 *Copy_pu8Data, uint8 Copy_u8SendAck);

#endif /* I2C_INTERFACE_H */
