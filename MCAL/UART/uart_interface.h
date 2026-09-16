#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "../../LIB/STD_TYPES.h"

/* ==================== UART Interrupt State ==================== */

#define UART_INTERRUPT_DISABLE    0U
#define UART_INTERRUPT_ENABLE     1U

/* ==================== Initialization ==================== */

STD_ReturnType UART_Init(uint32 Copy_u32BaudRate);

/* ==================== Transmission ==================== */

/*
 * Blocking byte transmission.
 * Used only for short console responses.
 */
STD_ReturnType UART_SendByte(uint8 Copy_u8Data);

STD_ReturnType UART_SendString(
    const uint8 *Copy_pu8String
);

/* ==================== Reception ==================== */

/*
 * Blocking reception.
 * Kept as a generic MCAL API.
 */
STD_ReturnType UART_ReceiveByte(
    uint8 *Copy_pu8Data
);

/*
 * Non-blocking reception from the RX ring buffer.
 */
STD_ReturnType UART_ReceiveByteNonBlocking(
    uint8 *Copy_pu8Data
);

/*
 * Returns E_OK when RX ring buffer contains data.
 */
STD_ReturnType UART_IsDataReady(void);

/* ==================== Interrupt Control ==================== */

STD_ReturnType UART_SetRxInterrupt(
    uint8 Copy_u8State
);

STD_ReturnType UART_SetTxInterrupt(
    uint8 Copy_u8State
);

/* ==================== RX Status ==================== */

/*
 * Returns and clears the RX overflow flag.
 */
uint8 UART_GetRxOverflow(void);

#endif /* UART_INTERFACE_H */