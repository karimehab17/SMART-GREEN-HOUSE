#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * MCAL UART — public API for the ATmega32 USART (8N1, polling).
 * Include this header from HAL, Logic, and main. Do not include UART_private.h there.
 *
 * Pins: RXD = PD0, TXD = PD1.
 */

#include "STD_TYPES.h"

/*
 * Description : Set 8 data bits, no parity, 1 stop bit (8N1), then enable TX and RX
 *               at Copy_u32BaudRate. UBRR = F_CPU / (16 * baud) - 1  (normal async).
 */
STD_ReturnType UART_Init(uint32 Copy_u32BaudRate);

/*
 * Description : Block until UDRE is set, then write one byte to UDR.
 */
STD_ReturnType UART_SendByte(uint8 Copy_u8Data);

/*
 * Description : Block until RXC is set, then read UDR into *Copy_pu8Data.
 */
STD_ReturnType UART_ReceiveByte(uint8 *Copy_pu8Data);

/*
 * Description : Send a NULL-terminated string with UART_SendByte.
 */
STD_ReturnType UART_SendString(const uint8 *Copy_pu8String);

/*
 * Description : Return E_OK if a byte is waiting (RXC = 1), else E_NOK.
 *               Does not read UDR.
 */
STD_ReturnType UART_IsDataReady(void);

/*
 * Description : Enable or disable RX complete / UDRE interrupts (RXCIE, UDRIE).
 *               1 = enable, 0 = disable. Call INTERRUPT_EnableGlobal after enabling.
 */
STD_ReturnType UART_SetRxInterrupt(uint8 Copy_u8State);
STD_ReturnType UART_SetTxInterrupt(uint8 Copy_u8State);

#endif /* UART_INTERFACE_H */
