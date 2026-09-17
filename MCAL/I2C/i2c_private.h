#ifndef I2C_PRIVATE_H
#define I2C_PRIVATE_H

#include "../../LIB/STD_TYPES.h"


/* =========================================================
 *                     TWI Registers
 * ========================================================= */

#define TWBR    (*(volatile uint8*)0x20U)
#define TWSR    (*(volatile uint8*)0x21U)
#define TWAR    (*(volatile uint8*)0x22U)
#define TWDR    (*(volatile uint8*)0x23U)
#define TWCR    (*(volatile uint8*)0x56U)


/* =========================================================
 *                       TWCR Bits
 * ========================================================= */

#define TWIE    0U
#define TWWC    3U
#define TWSTO   4U
#define TWSTA   5U
#define TWEA    6U
#define TWINT   7U
#define TWEN    2U


/* =========================================================
 *                        TWSR
 * ========================================================= */

#define TWI_PRESCALER_MASK   0x03U
#define TWS_STATUS_MASK      0xF8U

#define TWI_PRESCALER_1      0U


#endif /* I2C_PRIVATE_H */