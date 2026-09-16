#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "../../LIB/STD_TYPES.h"

/* ==================== UART Interrupt State ==================== */

#define UART_INTERRUPT_DISABLE    0U
#define UART_INTERRUPT_ENABLE     1U

/* ==================== Initialization ==================== */

STD_ReturnType UART_Init(uint32 Copy_u32BaudRate);

/* ==================== Blocking Transmission ==================== */

STD_ReturnType UART_SendByte(uint8 Copy_u8Data);

STD_ReturnType UART_SendString(
    const uint8 *Copy_pu8String
);

/* ==================== Non-Blocking Transmission ==================== */

STD_ReturnType UART_TxQueueByte(uint8 Copy_u8Data);

STD_ReturnType UART_TxQueueString(
    const uint8 *Copy_pu8String
);

STD_ReturnType UART_IsTxBusy(void);

/* ==================== Reception ==================== */

STD_ReturnType UART_ReceiveByte(
    uint8 *Copy_pu8Data
);

STD_ReturnType UART_ReceiveByteNonBlocking(
    uint8 *Copy_pu8Data
);

STD_ReturnType UART_IsDataReady(void);

/* ==================== Interrupt Control ==================== */

STD_ReturnType UART_SetRxInterrupt(
    uint8 Copy_u8State
);

STD_ReturnType UART_SetTxInterrupt(
    uint8 Copy_u8State
);

/* ==================== RX Status ==================== */

uint8 UART_GetRxOverflow(void);

#endif /* UART_INTERFACE_H */