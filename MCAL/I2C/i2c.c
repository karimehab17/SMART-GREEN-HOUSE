#include "../../LIB/STD_TYPES.h"
#include "config.h"
#include "i2c_interface.h"
#include "i2c_private.h"
#include "Math.h"

#include <stddef.h>


/* =========================================================
 *                     Private Helpers
 * ========================================================= */

static uint8 I2C_GetStatus(void)
{
    return (uint8)(TWSR & TWS_STATUS_MASK);
}


static uint8 I2C_IsValidAddress(uint8 Copy_u8Address)
{
    return (Copy_u8Address <= 0x7FU);
}


static void I2C_ClearPrescaler(void)
{
    TWSR &= (uint8)(~TWI_PRESCALER_MASK);
}


static void I2C_Enable(void)
{
    SET_BIT(TWCR, TWEN);
}


/* =========================================================
 *                     Initialization
 * ========================================================= */

STD_ReturnType I2C_InitMaster(uint32 Copy_u32SclHz)
{
    uint32 Local_u32TwbrValue;

    if (Copy_u32SclHz == 0U)
    {
        return E_NOK;
    }

    I2C_ClearPrescaler();

    if ((F_CPU / Copy_u32SclHz) < 16U)
    {
        return E_NOK;
    }

    Local_u32TwbrValue =
        ((F_CPU / Copy_u32SclHz) - 16U) / 2U;

    if (Local_u32TwbrValue > 255U)
    {
        return E_NOK;
    }

    TWBR = (uint8)Local_u32TwbrValue;

    TWCR = (1U << TWEN);

    return E_OK;
}


/* =========================================================
 *                         START
 * ========================================================= */

STD_ReturnType I2C_SendStart(void)
{
    TWCR = (1U << TWINT) |
           (1U << TWSTA) |
           (1U << TWEN);

    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for START completion */
    }

    if (I2C_GetStatus() != I2C_START_ACK)
    {
        I2C_SendStop();
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                    REPEATED START
 * ========================================================= */

STD_ReturnType I2C_SendRepeatedStart(void)
{
    TWCR = (1U << TWINT) |
           (1U << TWSTA) |
           (1U << TWEN);

    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for repeated START completion */
    }

    if (I2C_GetStatus() != I2C_REP_START_ACK)
    {
        I2C_SendStop();
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                           STOP
 * ========================================================= */

void I2C_SendStop(void)
{
    TWCR = (1U << TWINT) |
           (1U << TWSTO) |
           (1U << TWEN);
}


/* =========================================================
 *                       SLA + WRITE
 * ========================================================= */

STD_ReturnType I2C_SendSlaveAddressWithWrite(
    uint8 Copy_u8Address)
{
    if (I2C_IsValidAddress(Copy_u8Address) == 0U)
    {
        return E_NOK;
    }

    TWDR = (uint8)(Copy_u8Address << 1U);

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for address transmission */
    }

    if (I2C_GetStatus() != I2C_SLA_W_ACK)
    {
        I2C_SendStop();
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                       SLA + READ
 * ========================================================= */

STD_ReturnType I2C_SendSlaveAddressWithRead(
    uint8 Copy_u8Address)
{
    if (I2C_IsValidAddress(Copy_u8Address) == 0U)
    {
        return E_NOK;
    }

    TWDR = (uint8)((Copy_u8Address << 1U) | 1U);

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for address transmission */
    }

    if (I2C_GetStatus() != I2C_SLA_R_ACK)
    {
        I2C_SendStop();
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                         TRANSMIT
 * ========================================================= */

STD_ReturnType I2C_SendByte(uint8 Copy_u8Data)
{
    TWDR = Copy_u8Data;

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for data transmission */
    }

    if (I2C_GetStatus() != I2C_DATA_TX_ACK)
    {
        I2C_SendStop();
        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                          RECEIVE
 * ========================================================= */

STD_ReturnType I2C_ReceiveByte(
    uint8* Copy_pu8Data,
    uint8 Copy_u8SendAck)
{
    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    if ((Copy_u8SendAck != I2C_ACK) &&
        (Copy_u8SendAck != I2C_NACK))
    {
        return E_NOK;
    }

    if (Copy_u8SendAck == I2C_ACK)
    {
        TWCR = (1U << TWINT) |
               (1U << TWEA) |
               (1U << TWEN);
    }
    else
    {
        TWCR = (1U << TWINT) |
               (1U << TWEN);
    }

    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for data reception */
    }

    if (Copy_u8SendAck == I2C_ACK)
    {
        if (I2C_GetStatus() != I2C_DATA_RX_ACK)
        {
            I2C_SendStop();
            return E_NOK;
        }
    }
    else
    {
        if (I2C_GetStatus() != I2C_DATA_RX_NACK)
        {
            I2C_SendStop();
            return E_NOK;
        }
    }

    *Copy_pu8Data = TWDR;

    return E_OK;
}