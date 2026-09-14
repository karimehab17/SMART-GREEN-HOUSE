/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — I2C.c  (ATmega32 TWI master)
 * Implement every prototype from I2C_interface.h.
 */

#include "STD_TYPES.h"
#include "I2C_interface.h"
#include "I2C_private.h"

/*
 * I2C_InitMaster
 * 1. Reject SCL == 0.
 * 2. TWBR = ((F_CPU / Copy_u32SclHz) - 16) / 2.  TWSR prescaler bits = 00.
 * 3. TWCR = (1 << TWEN). Do not send START here.
 */

/*
 * I2C_SendStart
 * 1. TWCR = TWINT | TWSTA | TWEN.
 * 2. Wait for TWINT. Return E_OK only if status == I2C_START_ACK.
 */

/*
 * I2C_SendRepeatedStart
 * 1. Same as START, but expect I2C_REP_START_ACK (0x10).
 */

/*
 * I2C_SendStop
 * 1. TWCR = TWINT | TWSTO | TWEN. No status check.
 */

/*
 * I2C_SendSlaveAddressWithWrite
 * 1. TWDR = (Copy_u8Address << 1) | 0.
 * 2. TWCR = TWINT | TWEN. Expect I2C_SLA_W_ACK (0x18).
 *
 * I2C_SendSlaveAddressWithRead
 * 1. TWDR = (Copy_u8Address << 1) | 1.
 * 2. Expect I2C_SLA_R_ACK (0x40).
 */

/*
 * I2C_SendByte
 * 1. TWDR = Copy_u8Data. TWCR = TWINT | TWEN. Expect I2C_DATA_TX_ACK (0x28).
 */

/*
 * I2C_ReceiveByte
 * 1. Reject a NULL pointer.
 * 2. If Copy_u8SendAck == I2C_ACK: TWCR = TWINT | TWEA | TWEN, expect 0x50.
 *    If I2C_NACK:                 TWCR = TWINT | TWEN,        expect 0x58.
 * 3. *Copy_pu8Data = TWDR.
 */

/*
 * Typical 24Cxx write: START -> SLA+W -> word address -> data -> STOP
 * Typical 24Cxx read : START -> SLA+W -> word address -> REP START -> SLA+R -> data+NACK -> STOP
 */
