#include "control.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include "report.h"
#include <stddef.h>

#define PUMP_TIMEOUT_TICKS    6000U

static Config_t *g_pConfig = NULL;
static uint16 g_pumpRunTicks = 0U;
static uint8 g_pumpTimeoutLatched = 0U;


static CONTROL_Status_t CTRL_UpdatePumpProtection(
    ActuatorStateType Copy_PumpState)
{
    if (Copy_PumpState == ACT_STATE_ON)
    {
        if (g_pumpRunTicks < PUMP_TIMEOUT_TICKS)
        {
            g_pumpRunTicks++;
        }

        if (g_pumpRunTicks >= PUMP_TIMEOUT_TICKS)
        {
            (void)ACT_Set(
                ACTUATOR_PUMP,
                ACT_STATE_OFF
            );

            g_pumpRunTicks = 0U;
            g_pumpTimeoutLatched = 1U;

            (void)RPT_SendEvent(
                (const uint8 *)"PUMP,TIMEOUT"
            );
        }
    }
    else
    {
        g_pumpRunTicks = 0U;
    }

    return CONTROL_OK;
}


static CONTROL_Status_t CTRL_HandlePumpTimeout(
    uint8 Copy_u8SoilPct)
{
    if (g_pumpTimeoutLatched == 0U)
    {
        return CONTROL_OK;
    }

    if (ACT_Set(
        ACTUATOR_PUMP,
        ACT_STATE_OFF
    ) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Copy_u8SoilPct > g_pConfig->soilOffPct)
    {
        g_pumpTimeoutLatched = 0U;
        g_pumpRunTicks = 0U;
    }

    return CONTROL_OK;
}


CONTROL_Status_t CTRL_Init(Config_t *pConfig)
{
    if (pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (ACT_Init() != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_Init() != E_OK)
    {
        return CONTROL_ERROR;
    }

    g_pConfig = pConfig;

    g_pumpRunTicks = 0U;
    g_pumpTimeoutLatched = 0U;

    return CONTROL_OK;
}


CONTROL_Status_t CTRL_UpdateThermal(void)
{
    uint8 Local_u8TempC;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_GetTemperature(&Local_u8TempC) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Local_u8TempC > g_pConfig->tempOnC)
    {
        if (ACT_Set(
            ACTUATOR_FAN,
            ACT_STATE_ON
        ) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }
    else if (Local_u8TempC < g_pConfig->tempOffC)
    {
        if (ACT_Set(
            ACTUATOR_FAN,
            ACT_STATE_OFF
        ) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }

    return CONTROL_OK;
}


CONTROL_Status_t CTRL_UpdateIrrigation(void)
{
    uint16 Local_u16RawSoil = 0U;
    uint8 Local_u8SoilPct = 0U;
    ActuatorStateType Local_PumpState = ACT_STATE_OFF;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }
if (Sensors_GetSoil(
    &Local_u8SoilPct
) != E_OK)
{
    return CONTROL_ERROR;
}

    if (CTRL_HandlePumpTimeout(
        Local_u8SoilPct
    ) != CONTROL_OK)
    {
        return CONTROL_ERROR;
    }

    if (g_pumpTimeoutLatched != 0U)
    {
        return CONTROL_OK;
    }

    if (Local_u8SoilPct < g_pConfig->soilOnPct)
    {
        if (ACT_Set(
            ACTUATOR_PUMP,
            ACT_STATE_ON
        ) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }
    else if (Local_u8SoilPct > g_pConfig->soilOffPct)
    {
        if (ACT_Set(
            ACTUATOR_PUMP,
            ACT_STATE_OFF
        ) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }

    if (ACT_Get(
        ACTUATOR_PUMP,
        &Local_PumpState
    ) != E_OK)
    {
        return CONTROL_ERROR;
    }

    return CTRL_UpdatePumpProtection(
        Local_PumpState
    );
}


CONTROL_Status_t CTRL_UpdatePhoto(void)
{
    uint8 Local_u8LightPct;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_GetLight(
        &Local_u8LightPct
    ) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Local_u8LightPct < g_pConfig->lightOnPct)
    {
        if (ACT_Set(
            ACTUATOR_LAMP,
            ACT_STATE_ON
        ) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }
    else if (Local_u8LightPct > g_pConfig->lightOffPct)
    {
        if (ACT_Set(
            ACTUATOR_LAMP,
            ACT_STATE_OFF
        ) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }

    return CONTROL_OK;
}


CONTROL_Status_t CTRL_Update(void)
{
    if (CTRL_UpdateThermal() != CONTROL_OK)
    {
        return CONTROL_ERROR;
    }

    if (CTRL_UpdateIrrigation() != CONTROL_OK)
    {
        return CONTROL_ERROR;
    }

    if (CTRL_UpdatePhoto() != CONTROL_OK)
    {
        return CONTROL_ERROR;
    }

    return CONTROL_OK;
}