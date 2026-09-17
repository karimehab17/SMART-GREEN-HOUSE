#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"

#include "../../src/config.h"

#include "i2c_interface.h"
#include "i2c_private.h"

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
    /*
     * I2C uses a 7-bit slave address.
     */
    return (Copy_u8Address <= 0x7FU);
}


static void I2C_ClearPrescaler(void)
{
    /*
     * TWSR bits 1:0 select the TWI prescaler.
     */
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

    /*
     * SCL frequency must be non-zero.
     */
    if (Copy_u32SclHz == 0U)
    {
        return E_NOK;
    }

    /*
     * Use TWI prescaler = 1.
     */
    I2C_ClearPrescaler();

    /*
     * ATmega32A standard TWI formula:
     *
     * SCL = F_CPU / (16 + 2 * TWBR * Prescaler)
     *
     * With Prescaler = 1:
     *
     * TWBR = (F_CPU / SCL - 16) / 2
     */
    if ((F_CPU / Copy_u32SclHz) < 16U)
    {
        return E_NOK;
    }

    Local_u32TwbrValue =
        ((F_CPU / Copy_u32SclHz) - 16U) / 2U;

    /*
     * TWBR is an 8-bit register.
     */
    if (Local_u32TwbrValue > 255U)
    {
        return E_NOK;
    }

    TWBR = (uint8)Local_u32TwbrValue;

    /*
     * Enable TWI peripheral.
     */
    TWCR = (1U << TWEN);

    return E_OK;
}


/* =========================================================
 *                         START
 * ========================================================= */

STD_ReturnType I2C_SendStart(void)
{
    /*
     * Request START condition.
     */
    TWCR = (1U << TWINT) |
           (1U << TWSTA) |
           (1U << TWEN);

    /*
     * Wait until START has completed.
     */
    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for START completion. */
    }

    /*
     * Verify START status.
     */
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
    /*
     * Request repeated START condition.
     */
    TWCR = (1U << TWINT) |
           (1U << TWSTA) |
           (1U << TWEN);

    /*
     * Wait until repeated START has completed.
     */
    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for repeated START completion. */
    }

    /*
     * Verify repeated START status.
     */
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
    /*
     * Generate STOP condition.
     */
    TWCR = (1U << TWINT) |
           (1U << TWSTO) |
           (1U << TWEN);

    /*
     * Wait until STOP condition is completed.
     */
    while (GET_BIT(TWCR, TWSTO) != 0U)
    {
        /* Wait for STOP completion. */
    }
}


/* =========================================================
 *                    SLA + WRITE
 * ========================================================= */

STD_ReturnType I2C_SendSlaveAddressWithWrite(
    uint8 Copy_u8Address)
{
    /*
     * Validate 7-bit address.
     */
    if (I2C_IsValidAddress(Copy_u8Address) == 0U)
    {
        return E_NOK;
    }

    /*
     * Build SLA+W.
     *
     * 7-bit address << 1
     * R/W bit = 0
     */
    TWDR = (uint8)(Copy_u8Address << 1U);

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    /*
     * Wait for address transmission.
     */
    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for address transmission. */
    }

    /*
     * Verify SLA+W ACK.
     */
    if (I2C_GetStatus() != I2C_SLA_W_ACK)
    {
        I2C_SendStop();

        return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                     SLA + READ
 * ========================================================= */

STD_ReturnType I2C_SendSlaveAddressWithRead(
    uint8 Copy_u8Address)
{
    /*
     * Validate 7-bit address.
     */
    if (I2C_IsValidAddress(Copy_u8Address) == 0U)
    {
        return E_NOK;
    }

    /*
     * Build SLA+R.
     *
     * 7-bit address << 1
     * R/W bit = 1
     */
    TWDR = (uint8)((Copy_u8Address << 1U) | 1U);

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    /*
     * Wait for address transmission.
     */
    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for address transmission. */
    }

    /*
     * Verify SLA+R ACK.
     */
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
    /*
     * Load data into TWI data register.
     */
    TWDR = Copy_u8Data;

    TWCR = (1U << TWINT) |
           (1U << TWEN);

    /*
     * Wait for data transmission.
     */
    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for data transmission. */
    }

    /*
     * Verify data ACK.
     */
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
    /*
     * Validate destination pointer.
     */
    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    /*
     * Validate ACK/NACK selection.
     */
    if ((Copy_u8SendAck != I2C_ACK) &&
        (Copy_u8SendAck != I2C_NACK))
    {
        return E_NOK;
    }

    /*
     * Configure the response to the received byte.
     */
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

    /*
     * Wait for reception.
     */
    while (GET_BIT(TWCR, TWINT) == 0U)
    {
        /* Wait for data reception. */
    }

    /*
     * Validate received TWI status.
     */
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

    /*
     * Read received byte.
     */
    *Copy_pu8Data = TWDR;

    return E_OK;
}