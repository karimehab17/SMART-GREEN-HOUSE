#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "../../LIB/STD_TYPES.h"


/* =========================================================
 *                    TWI Status Codes
 * ========================================================= */

#define I2C_START_ACK         0x08U
#define I2C_REP_START_ACK     0x10U
#define I2C_SLA_W_ACK         0x18U
#define I2C_SLA_R_ACK         0x40U
#define I2C_DATA_TX_ACK       0x28U
#define I2C_DATA_RX_ACK       0x50U
#define I2C_DATA_RX_NACK      0x58U


/* =========================================================
 *                       ACK / NACK
 * ========================================================= */

#define I2C_ACK               1U
#define I2C_NACK              0U


/* =========================================================
 *                          API
 * ========================================================= */

STD_ReturnType I2C_InitMaster(
    uint32 Copy_u32SclHz
);


STD_ReturnType I2C_SendStart(void);


STD_ReturnType I2C_SendRepeatedStart(void);


void I2C_SendStop(void);


STD_ReturnType I2C_SendSlaveAddressWithWrite(
    uint8 Copy_u8Address
);


STD_ReturnType I2C_SendSlaveAddressWithRead(
    uint8 Copy_u8Address
);


STD_ReturnType I2C_SendByte(
    uint8 Copy_u8Data
);


STD_ReturnType I2C_ReceiveByte(
    uint8* Copy_pu8Data,
    uint8 Copy_u8SendAck
);


#endif /* I2C_INTERFACE_H */