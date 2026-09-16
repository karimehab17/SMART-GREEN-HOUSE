#include "console.h"

#include "../../MCAL/UART/UART_interface.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include "../Greenhouse_FSM/greenhouse_fsm.h"

#include <stddef.h>
#include <string.h>

static Config_t *g_pConfig = NULL;

static char g_commandBuffer[CONSOLE_MAX_COMMAND_LENGTH + 1U];
static uint8 g_commandLength = 0U;
static uint8 g_discardLine = 0U;

static void CON_SendText(const char *Copy_pText)
{
    if (Copy_pText != NULL)
    {
        (void)UART_SendString(
            (const uint8 *)Copy_pText);
    }
}

static void CON_SendOK(void)
{
    CON_SendText("OK\r\n");
}

static void CON_SendError(const char *Copy_pError)
{
    CON_SendText("ERR ");
    CON_SendText(Copy_pError);
    CON_SendText("\r\n");
}

static char CON_ToUpper(char Copy_cChar)
{
    if ((Copy_cChar >= 'a') &&
        (Copy_cChar <= 'z'))
    {
        Copy_cChar =
            (char)(Copy_cChar - 'a' + 'A');
    }

    return Copy_cChar;
}

static void CON_Normalize(void)
{
    uint8 Local_u8Index;

    for (Local_u8Index = 0U;
         Local_u8Index < g_commandLength;
         Local_u8Index++)
    {
        g_commandBuffer[Local_u8Index] =
            CON_ToUpper(
                g_commandBuffer[Local_u8Index]);
    }

    g_commandBuffer[g_commandLength] = '\0';
}

static STD_ReturnType CON_ParseNumber(
    const char *Copy_pText,
    uint16 *Copy_pu16Value)
{
    uint16 Local_u16Value = 0U;
    uint8 Local_u8Digits = 0U;

    if ((Copy_pText == NULL) ||
        (Copy_pu16Value == NULL))
    {
        return E_NOK;
    }

    while (*Copy_pText == ' ')
    {
        Copy_pText++;
    }

    while ((*Copy_pText >= '0') &&
           (*Copy_pText <= '9'))
    {
        Local_u16Value =
            (uint16)((Local_u16Value * 10U) +
                     (uint16)(*Copy_pText - '0'));

        Local_u8Digits++;
        Copy_pText++;
    }

    while (*Copy_pText == ' ')
    {
        Copy_pText++;
    }

    if ((Local_u8Digits == 0U) ||
        (*Copy_pText != '\0'))
    {
        return E_NOK;
    }

    *Copy_pu16Value = Local_u16Value;

    return E_OK;
}

static uint8 CON_ValidateConfig(
    const Config_t *Copy_pConfig)
{
    if (Copy_pConfig == NULL)
    {
        return 0U;
    }

    if ((Copy_pConfig->tempOnC < 10U) ||
        (Copy_pConfig->tempOnC > 50U))
    {
        return 0U;
    }

    if ((Copy_pConfig->tempOffC < 0U) ||
        (Copy_pConfig->tempOffC > 50U))
    {
        return 0U;
    }

    if (Copy_pConfig->tempOnC <=
        (uint8)(Copy_pConfig->tempOffC + 2U))
    {
        return 0U;
    }

    if ((Copy_pConfig->soilOnPct < 5U) ||
        (Copy_pConfig->soilOnPct > 95U))
    {
        return 0U;
    }

    if ((Copy_pConfig->soilOffPct < 5U) ||
        (Copy_pConfig->soilOffPct > 95U))
    {
        return 0U;
    }

    if ((Copy_pConfig->soilOnPct >=
         Copy_pConfig->soilOffPct))
    {
        return 0U;
    }

    if ((Copy_pConfig->lightOnPct > 100U) ||
        (Copy_pConfig->lightOffPct > 100U))
    {
        return 0U;
    }

    if (Copy_pConfig->lightOnPct >=
        Copy_pConfig->lightOffPct)
    {
        return 0U;
    }

    return 1U;
}

static CONSOLE_Status_t CON_HandleRead(
    const char *Copy_pCommand)
{
    uint8 Local_u8Value;

    if (strcmp(Copy_pCommand, "TEMP?") == 0)
    {
        if (Sensors_GetTemperature(
                &Local_u8Value) != E_OK)
        {
            return CONSOLE_ERROR;
        }

        CON_SendText("TEMP=");
        /* Numeric response handled below. */
        return CONSOLE_ERROR;
    }

    return CONSOLE_ERROR;
}

static void CON_SendUint8(uint8 Copy_u8Value)
{
    char Local_acBuffer[4];
    uint8 Local_u8Index = 0U;

    if (Copy_u8Value >= 100U)
    {
        Local_acBuffer[Local_u8Index++] =
            (char)('0' + (Copy_u8Value / 100U));

        Copy_u8Value =
            (uint8)(Copy_u8Value % 100U);
    }

    if ((Copy_u8Value >= 10U) ||
        (Local_u8Index != 0U))
    {
        Local_acBuffer[Local_u8Index++] =
            (char)('0' + (Copy_u8Value / 10U));

        Copy_u8Value =
            (uint8)(Copy_u8Value % 10U);
    }

    Local_acBuffer[Local_u8Index++] =
        (char)('0' + Copy_u8Value);

    Local_acBuffer[Local_u8Index] = '\0';

    CON_SendText(Local_acBuffer);
}

static void CON_SendConfig(void)
{
    CON_SendText("CFG=");

    CON_SendUint8(g_pConfig->tempOnC);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->tempOffC);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->soilOnPct);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->soilOffPct);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->lightOnPct);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->lightOffPct);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->tempAlarmC);
    CON_SendText(",");

    CON_SendUint8(g_pConfig->soilAlarmPct);

    CON_SendText("\r\n");
}

static CONSOLE_Status_t CON_HandleSensorCommand(
    const char *Copy_pCommand)
{
    uint8 Local_u8Value;

    if (strcmp(Copy_pCommand, "TEMP?") == 0)
    {
        if (Sensors_GetTemperature(
                &Local_u8Value) != E_OK)
        {
            return CONSOLE_ERROR;
        }

        CON_SendText("TEMP=");
        CON_SendUint8(Local_u8Value);
        CON_SendText("\r\n");

        return CONSOLE_OK;
    }

    if (strcmp(Copy_pCommand, "SOIL?") == 0)
    {
        if (Sensors_GetSoil(&Local_u8Value) != E_OK)
        {
            return CONSOLE_ERROR;
        }

        CON_SendText("SOIL=");
        CON_SendUint8(Local_u8Value);
        CON_SendText("\r\n");

        return CONSOLE_OK;
    }

    if (strcmp(Copy_pCommand, "LIGHT?") == 0)
    {
        if (Sensors_GetLight(&Local_u8Value) != E_OK)
        {
            return CONSOLE_ERROR;
        }

        CON_SendText("LIGHT=");
        CON_SendUint8(Local_u8Value);
        CON_SendText("\r\n");

        return CONSOLE_OK;
    }

    return CONSOLE_ERROR;
}

static CONSOLE_Status_t CON_HandleActuator(
    const char *Copy_pCommand)
{
    ActuatorType Local_enActuator;
    ActuatorStateType Local_enState;

    if (strncmp(Copy_pCommand, "FAN ", 4U) == 0)
    {
        Local_enActuator = ACTUATOR_FAN;
        Copy_pCommand += 4;
    }
    else if (strncmp(Copy_pCommand, "PUMP ", 5U) == 0)
    {
        Local_enActuator = ACTUATOR_PUMP;
        Copy_pCommand += 5;
    }
    else if (strncmp(Copy_pCommand, "LAMP ", 5U) == 0)
    {
        Local_enActuator = ACTUATOR_LAMP;
        Copy_pCommand += 5;
    }
    else
    {
        return CONSOLE_ERROR;
    }

    if (strcmp(Copy_pCommand, "ON") == 0)
    {
        Local_enState = ACT_STATE_ON;
    }
    else if (strcmp(Copy_pCommand, "OFF") == 0)
    {
        Local_enState = ACT_STATE_OFF;
    }
    else
    {
        CON_SendError("ARG");
        return CONSOLE_OK;
    }

    if (GHSM_GetState() != ST_MANUAL)
    {
        CON_SendError("MODE");
        return CONSOLE_OK;
    }

    if (ACT_Set(
            Local_enActuator,
            Local_enState) != E_OK)
    {
        return CONSOLE_ERROR;
    }

    CON_SendOK();

    return CONSOLE_OK;
}

static CONSOLE_Status_t CON_HandleSet(
    char *Copy_pCommand)
{
    char *Local_pName;
    char *Local_pValue;
    uint16 Local_u16Value;
    Config_t Local_Config;

    Local_pName = strtok(Copy_pCommand, " ");
    Local_pValue = strtok(NULL, " ");

    if ((Local_pName == NULL) ||
        (Local_pValue == NULL) ||
        (strtok(NULL, " ") != NULL))
    {
        CON_SendError("ARG");
        return CONSOLE_OK;
    }

    if (CON_ParseNumber(
            Local_pValue,
            &Local_u16Value) != E_OK)
    {
        CON_SendError("ARG");
        return CONSOLE_OK;
    }

    Local_Config = *g_pConfig;

    if (strcmp(Local_pName, "TEMPON") == 0)
    {
        Local_Config.tempOnC =
            (uint8)Local_u16Value;
    }
    else if (strcmp(Local_pName, "TEMPOFF") == 0)
    {
        Local_Config.tempOffC =
            (uint8)Local_u16Value;
    }
    else if (strcmp(Local_pName, "SOILON") == 0)
    {
        Local_Config.soilOnPct =
            (uint8)Local_u16Value;
    }
    else if (strcmp(Local_pName, "SOILOFF") == 0)
    {
        Local_Config.soilOffPct =
            (uint8)Local_u16Value;
    }
    else if (strcmp(Local_pName, "LIGHTON") == 0)
    {
        Local_Config.lightOnPct =
            (uint8)Local_u16Value;
    }
    else if (strcmp(Local_pName, "LIGHTOFF") == 0)
    {
        Local_Config.lightOffPct =
            (uint8)Local_u16Value;
    }
    else
    {
        CON_SendError("CMD");
        return CONSOLE_OK;
    }

    if (CON_ValidateConfig(&Local_Config) == 0U)
    {
        CON_SendError("RANGE");
        return CONSOLE_OK;
    }

    if (GHSM_RequestSetConfig(
            &Local_Config) != FSM_OK)
    {
        return CONSOLE_ERROR;
    }

    if (GHSM_RequestApply() != FSM_OK)
    {
        return CONSOLE_ERROR;
    }

    CON_SendOK();

    return CONSOLE_OK;
}

static void CON_HandleDefaults(void)
{
    Config_t Local_Config;

    Local_Config = *g_pConfig;

    Local_Config.magic = CFG_MAGIC;
    Local_Config.version = CFG_VERSION;

    Local_Config.tempOnC = DEFAULT_TEMP_ON_C;
    Local_Config.tempOffC = DEFAULT_TEMP_OFF_C;

    Local_Config.soilOnPct = DEFAULT_SOIL_ON_PCT;
    Local_Config.soilOffPct = DEFAULT_SOIL_OFF_PCT;

    Local_Config.lightOnPct = DEFAULT_LIGHT_ON_PCT;
    Local_Config.lightOffPct = DEFAULT_LIGHT_OFF_PCT;

    Local_Config.tempAlarmC = DEFAULT_TEMP_ALARM_C;
    Local_Config.soilAlarmPct =
        DEFAULT_SOIL_ALARM_PCT;

    if (GHSM_RequestSetConfig(
            &Local_Config) != FSM_OK)
    {
        CON_SendError("ARG");
        return;
    }

    if (GHSM_RequestApply() != FSM_OK)
    {
        CON_SendError("ARG");
        return;
    }

    CON_SendOK();
}

static CONSOLE_Status_t CON_HandleCommand(void)
{
    if (strcmp(g_commandBuffer, "HELP") == 0)
    {
        CON_SendText(
            "STATUS TEMP? SOIL? LIGHT? CFG? "
            "SET TEMPON/TEMPOFF/SOILON/SOILOFF/"
            "LIGHTON/LIGHTOFF MODE AUTO/MANUAL "
            "FAN/PUMP/LAMP ON/OFF RESET DEFAULTS HELP\r\n");

        return CONSOLE_OK;
    }

    if (strcmp(g_commandBuffer, "CFG?") == 0)
    {
        CON_SendConfig();
        return CONSOLE_OK;
    }

    if ((strcmp(g_commandBuffer, "TEMP?") == 0) ||
        (strcmp(g_commandBuffer, "SOIL?") == 0) ||
        (strcmp(g_commandBuffer, "LIGHT?") == 0))
    {
        return CON_HandleSensorCommand(
            g_commandBuffer);
    }

    if (strncmp(g_commandBuffer, "SET ", 4U) == 0)
    {
        return CON_HandleSet(
            &g_commandBuffer[4]);
    }

    if ((strncmp(g_commandBuffer, "FAN ", 4U) == 0) ||
        (strncmp(g_commandBuffer, "PUMP ", 5U) == 0) ||
        (strncmp(g_commandBuffer, "LAMP ", 5U) == 0))
    {
        return CON_HandleActuator(
            g_commandBuffer);
    }

    if (strcmp(g_commandBuffer, "DEFAULTS") == 0)
    {
        CON_HandleDefaults();
        return CONSOLE_OK;
    }

    if (strcmp(g_commandBuffer, "RESET") == 0)
    {
        if (GHSM_RequestReset() != FSM_OK)
        {
            return CONSOLE_ERROR;
        }

        CON_SendOK();
        return CONSOLE_OK;
    }

    CON_SendError("CMD");

    return CONSOLE_OK;
}

CONSOLE_Status_t CON_Init(void)
{
    g_pConfig = NULL;
    g_commandLength = 0U;
    g_discardLine = 0U;

    return CONSOLE_OK;
}

CONSOLE_Status_t CON_Process(void)
{
    uint8 Local_u8Byte;

    if (UART_IsDataReady() != E_OK)
    {
        return CONSOLE_OK;
    }

    if (UART_ReceiveByte(&Local_u8Byte) != E_OK)
    {
        return CONSOLE_ERROR;
    }

    if ((Local_u8Byte == '\r') ||
        (Local_u8Byte == '\n'))
    {
        if (g_discardLine != 0U)
        {
            g_discardLine = 0U;
            g_commandLength = 0U;
            return CONSOLE_OK;
        }

        if (g_commandLength == 0U)
        {
            return CONSOLE_OK;
        }

        g_commandBuffer[g_commandLength] = '\0';
        CON_Normalize();

        (void)CON_HandleCommand();

        g_commandLength = 0U;

        return CONSOLE_OK;
    }

    if (g_discardLine != 0U)
    {
        return CONSOLE_OK;
    }

    if (g_commandLength >=
        CONSOLE_MAX_COMMAND_LENGTH)
    {
        g_discardLine = 1U;
        CON_SendError("LONG");
        return CONSOLE_OK;
    }

    g_commandBuffer[g_commandLength++] =
        (char)Local_u8Byte;

    return CONSOLE_OK;
}