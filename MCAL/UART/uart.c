#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"
#include "config.h"
#include <stddef.h>

#include "UART_interface.h"
#include "UART_private.h"

#include <avr/interrupt.h>

/* =========================================================
 *                         Buffers
 * ========================================================= */

static volatile uint8 g_rxBuffer[UART_RX_BUFFER_SIZE];
static volatile uint8 g_rxHead = 0U;
static volatile uint8 g_rxTail = 0U;
static volatile uint8 g_rxOverflow = 0U;

static volatile uint8 g_txBuffer[UART_TX_BUFFER_SIZE];
static volatile uint8 g_txHead = 0U;
static volatile uint8 g_txTail = 0U;


/* =========================================================
 *                     Buffer Helpers
 * ========================================================= */

static uint8 UART_RxNext(uint8 Copy_u8Index)
{
    return (uint8)((Copy_u8Index + 1U) %
                   UART_RX_BUFFER_SIZE);
}


static uint8 UART_TxNext(uint8 Copy_u8Index)
{
    return (uint8)((Copy_u8Index + 1U) %
                   UART_TX_BUFFER_SIZE);
}


/* =========================================================
 *                       Initialization
 * ========================================================= */

STD_ReturnType UART_Init(uint32 Copy_u32BaudRate)
{
    uint16 Local_u16Ubr;

    if (Copy_u32BaudRate == 0UL)
    {
        return E_NOK;
    }

    Local_u16Ubr =
        (uint16)((F_CPU / (16UL * Copy_u32BaudRate)) - 1UL);

    UART_UCSRB_REG = 0U;

    UART_UBRRH_REG =
        (uint8)((Local_u16Ubr >> 8U) & 0x0FU);

    UART_UBRRL_REG =
        (uint8)(Local_u16Ubr & 0xFFU);

    UART_UCSRC_REG =
        (uint8)((1U << UART_URSEL_BIT) |
                (1U << UART_UCSZ1_BIT) |
                (1U << UART_UCSZ0_BIT));

    SET_BIT(UART_UCSRB_REG, UART_RXEN_BIT);
    SET_BIT(UART_UCSRB_REG, UART_TXEN_BIT);

    g_rxHead = 0U;
    g_rxTail = 0U;
    g_rxOverflow = 0U;

    g_txHead = 0U;
    g_txTail = 0U;

    return E_OK;
}


/* =========================================================
 *                   Blocking Transmission
 * ========================================================= */

STD_ReturnType UART_SendByte(uint8 Copy_u8Data)
{
    while (GET_BIT(UART_UCSRA_REG, UART_UDRE_BIT) == 0U)
    {
        /* Wait until transmit register is empty */
    }

    UART_UDR_REG = Copy_u8Data;

    return E_OK;
}


STD_ReturnType UART_SendString(
    const uint8 *Copy_pu8String)
{
    if (Copy_pu8String == NULL)
    {
        return E_NOK;
    }

    while (*Copy_pu8String != '\0')
    {
        (void)UART_SendByte(*Copy_pu8String);
        Copy_pu8String++;
    }

    return E_OK;
}


/* =========================================================
 *                  Non-Blocking Transmission
 * ========================================================= */

STD_ReturnType UART_TxQueueByte(uint8 Copy_u8Data)
{
    uint8 Local_u8Next;

    Local_u8Next = UART_TxNext(g_txHead);

    if (Local_u8Next == g_txTail)
    {
        return E_NOK;
    }

    g_txBuffer[g_txHead] = Copy_u8Data;
    g_txHead = Local_u8Next;

    SET_BIT(UART_UCSRB_REG, UART_UDRIE_BIT);

    return E_OK;
}


STD_ReturnType UART_TxQueueString(
    const uint8 *Copy_pu8String)
{
    const uint8 *Local_pu8Ptr;
    uint16 Local_u16Length;
    uint16 Local_u16Free;

    if (Copy_pu8String == NULL)
    {
        return E_NOK;
    }

    Local_pu8Ptr = Copy_pu8String;
    Local_u16Length = 0U;

    while (*Local_pu8Ptr != '\0')
    {
        Local_u16Length++;
        Local_pu8Ptr++;
    }

    if (Local_u16Length >= UART_TX_BUFFER_SIZE)
    {
        return E_NOK;
    }

    if (g_txHead >= g_txTail)
    {
        Local_u16Free =
            (uint16)UART_TX_BUFFER_SIZE -
            (uint16)(g_txHead - g_txTail) - 1U;
    }
    else
    {
        Local_u16Free =
            (uint16)(g_txTail - g_txHead) - 1U;
    }

    if (Local_u16Length > Local_u16Free)
    {
        return E_NOK;
    }

    while (*Copy_pu8String != '\0')
    {
        g_txBuffer[g_txHead] = *Copy_pu8String;
        g_txHead = UART_TxNext(g_txHead);
        Copy_pu8String++;
    }

    SET_BIT(UART_UCSRB_REG, UART_UDRIE_BIT);

    return E_OK;
}


STD_ReturnType UART_IsTxBusy(void)
{
    if (g_txHead != g_txTail)
    {
        return E_OK;
    }

    if (GET_BIT(UART_UCSRA_REG, UART_UDRE_BIT) == 0U)
    {
        return E_OK;
    }

    return E_NOK;
}


/* =========================================================
 *                         Reception
 * ========================================================= */

STD_ReturnType UART_IsDataReady(void)
{
    if (g_rxHead != g_rxTail)
    {
        return E_OK;
    }

    return E_NOK;
}


STD_ReturnType UART_ReceiveByteNonBlocking(
    uint8 *Copy_pu8Data)
{
    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    if (g_rxHead == g_rxTail)
    {
        return E_NOK;
    }

    *Copy_pu8Data = g_rxBuffer[g_rxTail];

    g_rxTail = UART_RxNext(g_rxTail);

    return E_OK;
}


STD_ReturnType UART_ReceiveByte(
    uint8 *Copy_pu8Data)
{
    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    while (g_rxHead == g_rxTail)
    {
        /* Wait for received data */
    }

    *Copy_pu8Data = g_rxBuffer[g_rxTail];

    g_rxTail = UART_RxNext(g_rxTail);

    return E_OK;
}


/* =========================================================
 *                     Interrupt Control
 * ========================================================= */

STD_ReturnType UART_SetRxInterrupt(
    uint8 Copy_u8State)
{
    if (Copy_u8State > UART_INTERRUPT_ENABLE)
    {
        return E_NOK;
    }

    if (Copy_u8State == UART_INTERRUPT_ENABLE)
    {
        SET_BIT(UART_UCSRB_REG, UART_RXCIE_BIT);
    }
    else
    {
        CLEAR_BIT(UART_UCSRB_REG, UART_RXCIE_BIT);
    }

    return E_OK;
}


STD_ReturnType UART_SetTxInterrupt(
    uint8 Copy_u8State)
{
    if (Copy_u8State > UART_INTERRUPT_ENABLE)
    {
        return E_NOK;
    }

    if (Copy_u8State == UART_INTERRUPT_ENABLE)
    {
        SET_BIT(UART_UCSRB_REG, UART_UDRIE_BIT);
    }
    else
    {
        CLEAR_BIT(UART_UCSRB_REG, UART_UDRIE_BIT);
    }

    return E_OK;
}


/* =========================================================
 *                       RX Status
 * ========================================================= */

uint8 UART_GetRxOverflow(void)
{
    return g_rxOverflow;
}


/* =========================================================
 *                           ISR
 * ========================================================= */

ISR(USART_RXC_vect)
{
    uint8 Local_u8Data;
    uint8 Local_u8Next;

    Local_u8Data = UART_UDR_REG;
    Local_u8Next = UART_RxNext(g_rxHead);

    if (Local_u8Next == g_rxTail)
    {
        g_rxOverflow = 1U;
    }
    else
    {
        g_rxBuffer[g_rxHead] = Local_u8Data;
        g_rxHead = Local_u8Next;
    }
}


ISR(USART_UDRE_vect)
{
    if (g_txHead == g_txTail)
    {
        CLEAR_BIT(UART_UCSRB_REG, UART_UDRIE_BIT);
    }
    else
    {
        UART_UDR_REG = g_txBuffer[g_txTail];
        g_txTail = UART_TxNext(g_txTail);
    }
}