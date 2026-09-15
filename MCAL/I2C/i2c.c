#include "STD_TYPES.h"
#include "I2C_interface.h"
#include "I2C_private.h"
#include <stddef.h>

/* =========================================================
 * I2C_InitMaster
 * ========================================================= */

STD_ReturnType I2C_InitMaster(uint32 Copy_u32SclHz)
{
    uint32 Local_u32TwbrValue;

    if (Copy_u32SclHz == 0u)
    {
        return E_NOK;
    }

    /*
     * TWPS1:0 = 00
     * Prescaler = 1
     */
    TWSR &= ~0x03u;

    /*
     * TWBR = ((F_CPU / SCL) - 16) / 2
     *
     * F_CPU = 8 MHz
     * SCL   = 100 kHz
     * TWBR  = 32
     */
    Local_u32TwbrValue =
        ((F_CPU / Copy_u32SclHz) - 16u) / 2u;

    TWBR = (uint8)Local_u32TwbrValue;

    /*
     * Enable TWI.
     * Do not send START here.
     */
    TWCR = (1u << TWEN);

    return E_OK;
}


/* =========================================================
 * I2C_SendStart
 * ========================================================= */

STD_ReturnType I2C_SendStart(void)
{
    /*
     * TWINT = 1 -> start next TWI action
     * TWSTA = 1 -> START condition
     * TWEN  = 1 -> enable TWI
     */
    TWCR = (1u << TWINT) |
           (1u << TWSTA) |
           (1u << TWEN);

    /* Wait until TWI operation completes */
    while ((TWCR & (1u << TWINT)) == 0u)
    {
        /* Wait */
    }

    if ((TWSR & TWS_STATUS_MASK) != I2C_START_ACK)
    {
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 * I2C_SendRepeatedStart
 * ========================================================= */

STD_ReturnType I2C_SendRepeatedStart(void)
{
    TWCR = (1u << TWINT) |
           (1u << TWSTA) |
           (1u << TWEN);

    while ((TWCR & (1u << TWINT)) == 0u)
    {
        /* Wait */
    }

    if ((TWSR & TWS_STATUS_MASK) != I2C_REP_START_ACK)
    {
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 * I2C_SendStop
 * ========================================================= */

void I2C_SendStop(void)
{
    /*
     * Generate STOP condition.
     */
    TWCR = (1u << TWINT) |
           (1u << TWSTO) |
           (1u << TWEN);
}


/* =========================================================
 * I2C_SendSlaveAddressWithWrite
 * ========================================================= */

STD_ReturnType I2C_SendSlaveAddressWithWrite(uint8 Copy_u8Address)
{
    /*
     * 7-bit address:
     *
     * bit 0 = 0 -> WRITE
     */
    TWDR = (uint8)((Copy_u8Address << 1u) | 0u);

    TWCR = (1u << TWINT) |
           (1u << TWEN);

    while ((TWCR & (1u << TWINT)) == 0u)
    {
        /* Wait */
    }

    if ((TWSR & TWS_STATUS_MASK) != I2C_SLA_W_ACK)
    {
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 * I2C_SendSlaveAddressWithRead
 * ========================================================= */

STD_ReturnType I2C_SendSlaveAddressWithRead(uint8 Copy_u8Address)
{
    /*
     * 7-bit address:
     *
     * bit 0 = 1 -> READ
     */
    TWDR = (uint8)((Copy_u8Address << 1u) | 1u);

    TWCR = (1u << TWINT) |
           (1u << TWEN);

    while ((TWCR & (1u << TWINT)) == 0u)
    {
        /* Wait */
    }

    if ((TWSR & TWS_STATUS_MASK) != I2C_SLA_R_ACK)
    {
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 * I2C_SendByte
 * ========================================================= */

STD_ReturnType I2C_SendByte(uint8 Copy_u8Data)
{
    TWDR = Copy_u8Data;

    TWCR = (1u << TWINT) |
           (1u << TWEN);

    while ((TWCR & (1u << TWINT)) == 0u)
    {
        /* Wait */
    }

    if ((TWSR & TWS_STATUS_MASK) != I2C_DATA_TX_ACK)
    {
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 * I2C_ReceiveByte
 * ========================================================= */

STD_ReturnType I2C_ReceiveByte(uint8 *Copy_pu8Data,
                               uint8 Copy_u8SendAck)
{
    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    /*
     * ACK:
     * More bytes are expected.
     */
    if (Copy_u8SendAck == I2C_ACK)
    {
        TWCR = (1u << TWINT) |
               (1u << TWEA) |
               (1u << TWEN);

        while ((TWCR & (1u << TWINT)) == 0u)
        {
            /* Wait */
        }

        if ((TWSR & TWS_STATUS_MASK) != I2C_DATA_RX_ACK)
        {
            return E_NOK;
        }
    }

    /*
     * NACK:
     * This is the last byte.
     */
    else if (Copy_u8SendAck == I2C_NACK)
    {
        TWCR = (1u << TWINT) |
               (1u << TWEN);

        while ((TWCR & (1u << TWINT)) == 0u)
        {
            /* Wait */
        }

        if ((TWSR & TWS_STATUS_MASK) != I2C_DATA_RX_NACK)
        {
            return E_NOK;
        }
    }

    else
    {
        return E_NOK;
    }

    *Copy_pu8Data = TWDR;

    return E_OK;
}