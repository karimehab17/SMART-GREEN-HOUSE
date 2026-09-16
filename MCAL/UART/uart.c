#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"
#include "../../LIB/config.h"

#include "UART_interface.h"
#include "UART_private.h"

#include <stddef.h>
#include <avr/interrupt.h>


/* =========================================================
 *                     RX Ring Buffer
 * ========================================================= */

#define UART_RX_BUFFER_SIZE    64U

static volatile uint8 g_uartRxBuffer[UART_RX_BUFFER_SIZE];

static volatile uint8 g_uartRxHead = 0U;
static volatile uint8 g_uartRxTail = 0U;

static volatile uint8 g_uartRxOverflow = 0U;


/* =========================================================
 *                     Private Helpers
 * ========================================================= */

static uint8 UART_NextIndex(uint8 Copy_u8Index)
{
    Copy_u8Index++;

    if (Copy_u8Index >= UART_RX_BUFFER_SIZE)
    {
        Copy_u8Index = 0U;
    }

    return Copy_u8Index;
}


static uint8 UART_RxBufferIsEmpty(void)
{
    return (
        g_uartRxHead == g_uartRxTail
    );
}


/* =========================================================
 *                       Initialization
 * ========================================================= */

STD_ReturnType UART_Init(uint32 Copy_u32BaudRate)
{
    uint16 local_u16Ubr;

    if (Copy_u32BaudRate == 0UL)
    {
        return E_NOK;
    }

    /*
     * Normal asynchronous mode:
     *
     * UBRR = F_CPU / (16 * Baud) - 1
     *
     * At 8 MHz / 9600:
     * UBRR = 51
     */
    local_u16Ubr =
        (uint16)(
            (F_CPU /
            (16UL * Copy_u32BaudRate)) - 1UL
        );

    /* Disable RX/TX during configuration */
    UART_UCSRB_REG = 0U;

    /*
     * UBRRH shares address with UCSRC.
     * URSEL = 0 selects UBRRH.
     */
    UART_UBRRH_REG =
        (uint8)(local_u16Ubr >> 8U);

    UART_UBRRL_REG =
        (uint8)(local_u16Ubr & 0xFFU);

    /*
     * UCSRC:
     * URSEL = 1
     * UMSEL = 0  -> asynchronous
     * UPM = 00    -> no parity
     * USBS = 0    -> 1 stop bit
     * UCSZ1:0=11  -> 8 data bits
     */
    UART_UCSRC_REG =
        (uint8)(
            (1U << UART_URSEL_BIT) |
            (1U << UART_UCSZ1_BIT) |
            (1U << UART_UCSZ0_BIT)
        );

    /*
     * Enable receiver and transmitter.
     * RX interrupt is enabled separately.
     */
    UART_UCSRB_REG =
        (uint8)(
            (1U << UART_RXEN_BIT) |
            (1U << UART_TXEN_BIT)
        );

    g_uartRxHead = 0U;
    g_uartRxTail = 0U;
    g_uartRxOverflow = 0U;

    return E_OK;
}


/* =========================================================
 *                         TX
 * ========================================================= */

STD_ReturnType UART_SendByte(uint8 Copy_u8Data)
{
    while (
        (UART_UCSRA_REG &
        (1U << UART_UDRE_BIT)) == 0U
    )
    {
        /* Wait for transmit register */
    }

    UART_UDR_REG = Copy_u8Data;

    return E_OK;
}


STD_ReturnType UART_SendString(
    const uint8 *Copy_pu8String)
{
    uint8 local_u8Index = 0U;

    if (Copy_pu8String == NULL)
    {
        return E_NOK;
    }

    while (Copy_pu8String[local_u8Index] != '\0')
    {
        if (
            UART_SendByte(
                Copy_pu8String[local_u8Index]
            ) != E_OK
        )
        {
            return E_NOK;
        }

        local_u8Index++;
    }

    return E_OK;
}


/* =========================================================
 *                  Blocking RX
 * ========================================================= */

STD_ReturnType UART_ReceiveByte(
    uint8 *Copy_pu8Data)
{
    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    while (
        (UART_UCSRA_REG &
        (1U << UART_RXC_BIT)) == 0U
    )
    {
        /* Wait for received byte */
    }

    *Copy_pu8Data = UART_UDR_REG;

    return E_OK;
}


/* =========================================================
 *                Non-Blocking RX
 * ========================================================= */

STD_ReturnType UART_ReceiveByteNonBlocking(
    uint8 *Copy_pu8Data)
{
    uint8 local_u8Tail;

    if (Copy_pu8Data == NULL)
    {
        return E_NOK;
    }

    if (g_uartRxHead == g_uartRxTail)
    {
        return E_NOK;
    }

    local_u8Tail = g_uartRxTail;

    *Copy_pu8Data =
        g_uartRxBuffer[local_u8Tail];

    g_uartRxTail =
        UART_NextIndex(local_u8Tail);

    return E_OK;
}


/* =========================================================
 *                     RX Ready
 * ========================================================= */

STD_ReturnType UART_IsDataReady(void)
{
    if (g_uartRxHead != g_uartRxTail)
    {
        return E_OK;
    }

    return E_NOK;
}


/* =========================================================
 *                 RX Interrupt Control
 * ========================================================= */

STD_ReturnType UART_SetRxInterrupt(
    uint8 Copy_u8State)
{
    if (
        Copy_u8State != UART_INTERRUPT_ENABLE &&
        Copy_u8State != UART_INTERRUPT_DISABLE
    )
    {
        return E_NOK;
    }

    if (Copy_u8State == UART_INTERRUPT_ENABLE)
    {
        SET_BIT(
            UART_UCSRB_REG,
            UART_RXCIE_BIT
        );
    }
    else
    {
        CLEAR_BIT(
            UART_UCSRB_REG,
            UART_RXCIE_BIT
        );
    }

    return E_OK;
}


/* =========================================================
 *                 TX Interrupt Control
 * ========================================================= */

STD_ReturnType UART_SetTxInterrupt(
    uint8 Copy_u8State)
{
    if (
        Copy_u8State != UART_INTERRUPT_ENABLE &&
        Copy_u8State != UART_INTERRUPT_DISABLE
    )
    {
        return E_NOK;
    }

    if (Copy_u8State == UART_INTERRUPT_ENABLE)
    {
        SET_BIT(
            UART_UCSRB_REG,
            UART_UDRIE_BIT
        );
    }
    else
    {
        CLEAR_BIT(
            UART_UCSRB_REG,
            UART_UDRIE_BIT
        );
    }

    return E_OK;
}


/* =========================================================
 *                    RX Overflow
 * ========================================================= */

uint8 UART_GetRxOverflow(void)
{
    uint8 local_u8Overflow;

    local_u8Overflow = g_uartRxOverflow;

    g_uartRxOverflow = 0U;

    return local_u8Overflow;
}


/* =========================================================
 *                       RX ISR
 * ========================================================= */

ISR(USART_RXC_vect)
{
    uint8 local_u8Next;

    local_u8Next =
        UART_NextIndex(g_uartRxHead);

    if (local_u8Next != g_uartRxTail)
    {
        g_uartRxBuffer[g_uartRxHead] =
            UART_UDR_REG;

        g_uartRxHead = local_u8Next;
    }
    else
    {
        /*
         * Buffer full.
         * Read UDR to clear the RX condition.
         */
        (void)UART_UDR_REG;

        g_uartRxOverflow = 1U;
    }
}