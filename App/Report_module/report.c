#include "report.h"

#include "../../MCAL/UART/UART_interface.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include "../Greenhouse_FSM/greenhouse_fsm.h"

#include <stddef.h>


#define RPT_FRAME_BUFFER_SIZE    80U
#define RPT_EVENT_BUFFER_SIZE    32U
#define RPT_MAX_UINT16           ((uint16)(~0U))


static SysData_t *g_pSysData = NULL;

static uint8 g_frameBuffer[RPT_FRAME_BUFFER_SIZE];
static uint8 g_eventBuffer[RPT_EVENT_BUFFER_SIZE];

static uint16 g_pumpRuntimeSec = 0U;


/* =========================================================
 *                     Private Prototypes
 * ========================================================= */

static uint8 RPT_ToHex(uint8 Copy_u8Value);

static uint8 RPT_CalculateChecksum(
    const uint8 *pData);

static uint16 RPT_AppendText(
    uint16 Copy_u16Index,
    const uint8 *pText);

static uint16 RPT_AppendNumber(
    uint16 Copy_u16Index,
    uint16 Copy_u16Number);

static const uint8 *RPT_GetModeName(void);

static STD_ReturnType RPT_QueueBuffer(
    const uint8 *pBuffer);

static STD_ReturnType RPT_UpdateSystemData(void);

static void RPT_UpdatePumpRuntime(void);

static STD_ReturnType RPT_AppendStatus(
    uint16 *pIndex);


/* =========================================================
 *                         HEX Helper
 * ========================================================= */

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


/* =========================================================
 *                       Checksum
 * ========================================================= */

static uint8 RPT_CalculateChecksum(
    const uint8 *pData)
{
    uint8 Local_u8Checksum = 0U;
    uint8 Local_u8Index = 0U;

    if (pData == NULL)
    {
        return 0U;
    }

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


/* =========================================================
 *                       Append Text
 * ========================================================= */

static uint16 RPT_AppendText(
    uint16 Copy_u16Index,
    const uint8 *pText)
{
    uint16 Local_u16TextIndex = 0U;

    if (pText == NULL)
    {
        return Copy_u16Index;
    }

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


/* =========================================================
 *                     Append Number
 * ========================================================= */

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


/* =========================================================
 *                       Mode Name
 * ========================================================= */

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

        case ST_CONFIG:
            return (const uint8 *)"CONFIG";

        case ST_ALARM:
            return (const uint8 *)"AUTO";

        default:
            return (const uint8 *)"INIT";
    }
}


/* =========================================================
 *                       UART Queue
 * ========================================================= */

static STD_ReturnType RPT_QueueBuffer(
    const uint8 *pBuffer)
{
    if (pBuffer == NULL)
    {
        return E_NOK;
    }

    return UART_TxQueueString(pBuffer);
}


/* =========================================================
 *                  Update System Data
 * ========================================================= */

static STD_ReturnType RPT_UpdateSystemData(void)
{
    ActuatorStateType Local_State;

    if (g_pSysData == NULL)
    {
        return E_NOK;
    }

    if (Sensors_GetTemperature(
            &g_pSysData->tempC) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_GetSoil(
            &g_pSysData->soilPct) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_GetLight(
            &g_pSysData->lightPct) != E_OK)
    {
        return E_NOK;
    }

    if (ACT_Get(
            ACTUATOR_FAN,
            &Local_State) != E_OK)
    {
        return E_NOK;
    }

    g_pSysData->fanOn =
        (uint8)Local_State;

    if (ACT_Get(
            ACTUATOR_PUMP,
            &Local_State) != E_OK)
    {
        return E_NOK;
    }

    g_pSysData->pumpOn =
        (uint8)Local_State;

    if (ACT_Get(
            ACTUATOR_LAMP,
            &Local_State) != E_OK)
    {
        return E_NOK;
    }

    g_pSysData->lampOn =
        (uint8)Local_State;

    if (ACT_Get(
            ACTUATOR_ALARM,
            &Local_State) != E_OK)
    {
        return E_NOK;
    }

    g_pSysData->alarmOn =
        (uint8)Local_State;

    g_pSysData->mode =
        (uint8)GHSM_GetState();

    return E_OK;
}


/* =========================================================
 *                    Pump Runtime
 * ========================================================= */

static void RPT_UpdatePumpRuntime(void)
{
    uint16 Local_u16Increment;

    Local_u16Increment =
        (uint16)(
            SCH_REPORT_PERIOD_MS / 1000U
        );

    if (g_pSysData->pumpOn == ACTUATOR_ON)
    {
        if (
            g_pumpRuntimeSec <=
            (uint16)(
                RPT_MAX_UINT16 -
                Local_u16Increment
            )
        )
        {
            g_pumpRuntimeSec =
                (uint16)(
                    g_pumpRuntimeSec +
                    Local_u16Increment
                );
        }
        else
        {
            g_pumpRuntimeSec =
                RPT_MAX_UINT16;
        }
    }

    if (
        g_pSysData->upTimeSec <=
        (uint16)(
            RPT_MAX_UINT16 -
            Local_u16Increment
        )
    )
    {
        g_pSysData->upTimeSec =
            (uint16)(
                g_pSysData->upTimeSec +
                Local_u16Increment
            );
    }
    else
    {
        g_pSysData->upTimeSec =
            RPT_MAX_UINT16;
    }
}


/* =========================================================
 *                  Append Status Fields
 * ========================================================= */

static STD_ReturnType RPT_AppendStatus(
    uint16 *pIndex)
{
    if (pIndex == NULL)
    {
        return E_NOK;
    }

    *pIndex = RPT_AppendText(
        *pIndex,
        (const uint8 *)"$GH,T="
    );

    *pIndex = RPT_AppendNumber(
        *pIndex,
        g_pSysData->tempC
    );

    *pIndex = RPT_AppendText(
        *pIndex,
        (const uint8 *)",PUMPSEC="
    );

    *pIndex = RPT_AppendNumber(
        *pIndex,
        g_pumpRuntimeSec
    );

    *pIndex = RPT_AppendText(
        *pIndex,
        (const uint8 *)",S="
    );

    *pIndex = RPT_AppendNumber(
        *pIndex,
        g_pSysData->soilPct
    );

    *pIndex = RPT_AppendText(
        *pIndex,
        (const uint8 *)",L="
    );

    *pIndex = RPT_AppendNumber(
        *pIndex,
        g_pSysData->lightPct
    );

    return E_OK;
}


/* =========================================================
 *                    Send Status
 * ========================================================= */

STD_ReturnType RPT_SendStatus(void)
{
    uint16 Local_u16Index = 0U;
    uint8 Local_u8Checksum;

    if (g_pSysData == NULL)
    {
        return E_NOK;
    }

    if (RPT_AppendStatus(
            &Local_u16Index) != E_OK)
    {
        return E_NOK;
    }

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
        RPT_CalculateChecksum(
            &g_frameBuffer[1]
        );

    g_frameBuffer[Local_u16Index++] = '*';

    g_frameBuffer[Local_u16Index++] =
        RPT_ToHex(
            (uint8)(
                Local_u8Checksum >> 4U
            )
        );

    g_frameBuffer[Local_u16Index++] =
        RPT_ToHex(Local_u8Checksum);

    g_frameBuffer[Local_u16Index++] = '\r';
    g_frameBuffer[Local_u16Index++] = '\n';

    g_frameBuffer[Local_u16Index] = '\0';

    return RPT_QueueBuffer(
        g_frameBuffer
    );
}


/* =========================================================
 *                         Init
 * ========================================================= */

STD_ReturnType RPT_Init(SysData_t *pSysData)
{
    if (pSysData == NULL)
    {
        return E_NOK;
    }

    g_pSysData = pSysData;

    g_pumpRuntimeSec = 0U;

    g_pSysData->tempC = 0U;
    g_pSysData->soilPct = 0U;
    g_pSysData->lightPct = 0U;

    g_pSysData->fanOn = ACTUATOR_OFF;
    g_pSysData->pumpOn = ACTUATOR_OFF;
    g_pSysData->lampOn = ACTUATOR_OFF;
    g_pSysData->alarmOn = ACTUATOR_OFF;

    g_pSysData->mode =
        (uint8)ST_INIT;

    g_pSysData->upTimeSec = 0U;

    return E_OK;
}


/* =========================================================
 *                         Update
 * ========================================================= */

STD_ReturnType RPT_Update(void)
{
    if (g_pSysData == NULL)
    {
        return E_NOK;
    }

    if (RPT_UpdateSystemData() != E_OK)
    {
        return E_NOK;
    }

    RPT_UpdatePumpRuntime();

    return RPT_SendStatus();
}


/* =========================================================
 *                         Events
 * ========================================================= */

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
        g_eventBuffer[
            Local_u16Index + 5U
        ] = pEvent[Local_u16Index];

        Local_u16Index++;
    }

    g_eventBuffer[
        Local_u16Index + 5U
    ] = '\r';

    g_eventBuffer[
        Local_u16Index + 6U
    ] = '\n';

    g_eventBuffer[
        Local_u16Index + 7U
    ] = '\0';

    return RPT_QueueBuffer(
        g_eventBuffer
    );
}


/* =========================================================
 *                    Pump Runtime Getter
 * ========================================================= */

uint16 RPT_GetPumpRuntime(void)
{
    return g_pumpRuntimeSec;
}