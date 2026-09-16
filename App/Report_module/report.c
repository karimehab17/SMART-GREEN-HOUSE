#include "report.h"

#include "../../MCAL/UART/UART_interface.h"
#include "greenhouse_fsm.h"

#include <stddef.h>


#define RPT_FRAME_BUFFER_SIZE    64U
#define RPT_EVENT_BUFFER_SIZE    32U

static SysData_t *g_pSysData = NULL;

static uint8 g_frameBuffer[RPT_FRAME_BUFFER_SIZE];
static uint8 g_eventBuffer[RPT_EVENT_BUFFER_SIZE];

static uint16 g_pumpRuntimeSec = 0U;
static uint8 g_lastPumpState = 0U;


static uint8 RPT_ToHex(uint8 Copy_u8Value)
{
    Copy_u8Value &= 0x0FU;

    if (Copy_u8Value < 10U)
    {
        return (uint8)('0' + Copy_u8Value);
    }

    return (uint8)(
        'A' + (Copy_u8Value - 10U)
    );
}


static uint8 RPT_CalculateChecksum(
    const uint8 *pData)
{
    uint8 Local_u8Checksum = 0U;
    uint8 Local_u8Index = 0U;

    while (
        (pData[Local_u8Index] != '\0') &&
        (Local_u8Index <
         (RPT_FRAME_BUFFER_SIZE - 1U))
    )
    {
        Local_u8Checksum ^=
            pData[Local_u8Index];

        Local_u8Index++;
    }

    return Local_u8Checksum;
}


static uint16 RPT_AppendText(
    uint16 Copy_u16Index,
    const uint8 *pText)
{
    uint16 Local_u16TextIndex = 0U;

    while (
        (pText[Local_u16TextIndex] != '\0') &&
        (Copy_u16Index <
         (RPT_FRAME_BUFFER_SIZE - 1U))
    )
    {
        g_frameBuffer[Copy_u16Index] =
            pText[Local_u16TextIndex];

        Copy_u16Index++;
        Local_u16TextIndex++;
    }

    return Copy_u16Index;
}


static uint16 RPT_AppendNumber(
    uint16 Copy_u16Index,
    uint16 Copy_u16Number)
{
    uint8 Local_u8Digits[5U];
    uint8 Local_u8Count = 0U;

    do
    {
        Local_u8Digits[Local_u8Count] =
            (uint8)(Copy_u16Number % 10U);

        Copy_u16Number /= 10U;
        Local_u8Count++;

    } while (
        (Copy_u16Number > 0U) &&
        (Local_u8Count < 5U)
    );

    while (Local_u8Count > 0U)
    {
        Local_u8Count--;

        if (
            Copy_u16Index <
            (RPT_FRAME_BUFFER_SIZE - 1U)
        )
        {
            g_frameBuffer[Copy_u16Index] =
                (uint8)(
                    '0' +
                    Local_u8Digits[Local_u8Count]
                );

            Copy_u16Index++;
        }
    }

    return Copy_u16Index;
}


static const uint8 *RPT_GetModeName(void)
{
    if (g_pSysData == NULL)
    {
        return (const uint8 *)"INIT";
    }

    switch (g_pSysData->mode)
    {
        case ST_AUTO:
            return (const uint8 *)"AUTO";

        case ST_MANUAL:
            return (const uint8 *)"MANUAL";

        case ST_ALARM:
            return (const uint8 *)"ALARM";

        case ST_CONFIG:
            return (const uint8 *)"CONFIG";

        default:
            return (const uint8 *)"INIT";
    }
}


static STD_ReturnType RPT_QueueBuffer(
    const uint8 *pBuffer)
{
    if (pBuffer == NULL)
    {
        return E_NOK;
    }

    return UART_TxQueueString(pBuffer);
}


STD_ReturnType RPT_Init(SysData_t *pSysData)
{
    if (pSysData == NULL)
    {
        return E_NOK;
    }

    g_pSysData = pSysData;
    g_pumpRuntimeSec = 0U;
    g_lastPumpState = 0U;

    return E_OK;
}


STD_ReturnType RPT_Update(void)
{
    if (g_pSysData == NULL)
    {
        return E_NOK;
    }

    return RPT_SendStatus();
}


STD_ReturnType RPT_SendStatus(void)
{
    uint16 Local_u16Index = 0U;
    uint8 Local_u8Checksum;

    if (g_pSysData == NULL)
    {
        return E_NOK;
    }

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)"$GH,T="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->tempC
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",S="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->soilPct
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",L="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->lightPct
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",F="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->fanOn
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",P="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->pumpOn
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",M="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->lampOn
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",A="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->alarmOn
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",MODE="
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            RPT_GetModeName()
        );

    Local_u16Index =
        RPT_AppendText(
            Local_u16Index,
            (const uint8 *)",UP="
        );

    Local_u16Index =
        RPT_AppendNumber(
            Local_u16Index,
            g_pSysData->upTimeSec
        );

    g_frameBuffer[Local_u16Index] = '\0';

    Local_u8Checksum =
        RPT_CalculateChecksum(g_frameBuffer);

    g_frameBuffer[Local_u16Index++] = '*';

    g_frameBuffer[Local_u16Index++] =
        RPT_ToHex(
            (uint8)(Local_u8Checksum >> 4U)
        );

    g_frameBuffer[Local_u16Index++] =
        RPT_ToHex(Local_u8Checksum);

    g_frameBuffer[Local_u16Index++] = '\r';
    g_frameBuffer[Local_u16Index++] = '\n';
    g_frameBuffer[Local_u16Index] = '\0';

    return RPT_QueueBuffer(g_frameBuffer);
}


STD_ReturnType RPT_SendEvent(
    const uint8 *pEvent)
{
    uint16 Local_u16Index = 0U;

    if (pEvent == NULL)
    {
        return E_NOK;
    }

    g_eventBuffer[0] = '!';
    g_eventBuffer[1] = 'E';
    g_eventBuffer[2] = 'V';
    g_eventBuffer[3] = 'T';
    g_eventBuffer[4] = ',';

    while (
        (pEvent[Local_u16Index] != '\0') &&
        (Local_u16Index <
         (RPT_EVENT_BUFFER_SIZE - 8U))
    )
    {
        g_eventBuffer[Local_u16Index + 5U] =
            pEvent[Local_u16Index];

        Local_u16Index++;
    }

    g_eventBuffer[Local_u16Index + 5U] = '\r';
    g_eventBuffer[Local_u16Index + 6U] = '\n';
    g_eventBuffer[Local_u16Index + 7U] = '\0';

    return RPT_QueueBuffer(g_eventBuffer);
}


uint16 RPT_GetPumpRuntime(void)
{
    return g_pumpRuntimeSec;
}