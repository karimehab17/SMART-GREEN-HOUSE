/*
 * Author: Ahmed Ellamie
 * Email:  ahmed.ellamiee@gmail.com
 *
 * STUDENT TASK — UART.c  (ATmega32 USART, 8N1 polling)
 * Implement every prototype from UART_interface.h.
 */

#include "STD_TYPES.h"
#include "UART_interface.h"
#include "UART_private.h"
#include "stddef.h"

/*
 * UART_Init
 * 1. Reject baud == 0.
 * 2. Compute UBRR = F_CPU / (16 * baud) - 1. Write UBRRH then UBRRL.
 * 3. UCSRC = URSEL | UCSZ1 | UCSZ0   (8N1, async).
 * 4. UCSRB = RXEN | TXEN.
 * 5. At 8 MHz, 9600 baud -> UBRR = 51.
 */

/*
 * UART_SendByte
 * 1. while (UDRE == 0) ;   then UDR = Copy_u8Data.
 */

/*
 * UART_ReceiveByte
 * 1. Reject a NULL pointer.
 * 2. while (RXC == 0) ;    then *Copy_pu8Data = UDR.
 */

/*
 * UART_SendString
 * 1. Reject a NULL pointer.
 * 2. Send bytes until '\0'. Do not send the terminator unless the lab asks.
 */

/*
 * UART_IsDataReady
 * 1. Return E_OK if RXC is 1, else E_NOK.
 */

/*
 * UART_SetRxInterrupt / UART_SetTxInterrupt
 * 1. Set or clear RXCIE / UDRIE in UCSRB.
 * 2. Vectors: USART_RXC_vect , USART_UDRE_vect.
 */
