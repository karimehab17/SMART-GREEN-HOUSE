#ifndef UART_PRIVATE_H
#define UART_PRIVATE_H

#include "../../LIB/STD_TYPES.h"

/* ==================== USART Registers ==================== */

#define UART_UDR_REG      (*(volatile uint8*)0x2CU)
#define UART_UCSRA_REG    (*(volatile uint8*)0x2BU)
#define UART_UCSRB_REG    (*(volatile uint8*)0x2AU)
#define UART_UCSRC_REG    (*(volatile uint8*)0x40U)
#define UART_UBRRL_REG    (*(volatile uint8*)0x29U)
#define UART_UBRRH_REG    (*(volatile uint8*)0x40U)

/* ==================== UCSRA Bits ==================== */

#define UART_RXC_BIT      7U
#define UART_TXC_BIT      6U
#define UART_UDRE_BIT     5U
#define UART_FE_BIT       4U
#define UART_DOR_BIT      3U
#define UART_PE_BIT       2U
#define UART_U2X_BIT      1U
#define UART_MPCM_BIT     0U

/* ==================== UCSRB Bits ==================== */

#define UART_RXCIE_BIT    7U
#define UART_TXCIE_BIT    6U
#define UART_UDRIE_BIT    5U
#define UART_RXEN_BIT     4U
#define UART_TXEN_BIT     3U
#define UART_UCSZ2_BIT    2U
#define UART_RXB8_BIT     1U
#define UART_TXB8_BIT     0U

/* ==================== UCSRC Bits ==================== */

#define UART_URSEL_BIT    7U
#define UART_UMSEL_BIT    6U
#define UART_UPM1_BIT     5U
#define UART_UPM0_BIT     4U
#define UART_USBS_BIT     3U
#define UART_UCSZ1_BIT    2U
#define UART_UCSZ0_BIT    1U
#define UART_UCPOL_BIT    0U

#endif /* UART_PRIVATE_H */