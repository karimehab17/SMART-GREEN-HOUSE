#include "control.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include <stddef.h>
#include "report.h"

#define PUMP_TIMEOUT_TICKS    6000U

static Config_t *g_pConfig = NULL;
static uint16 g_pumpRunTicks = 0U;
static uint8 g_pumpTimeoutLatched = 0U;

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
        if (ACT_Set(ACTUATOR_FAN, ACT_STATE_ON) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }
    else if (Local_u8TempC < g_pConfig->tempOffC)
    {
        if (ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }

    return CONTROL_OK;
}

CONTROL_Status_t CTRL_UpdateIrrigation(void)
{
    uint16 rawSoil = 0U;
    uint8 soilPct = 0U;
    ActuatorStateType pumpState = ACT_STATE_OFF;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_ReadRaw(NULL, &rawSoil, NULL) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_ScalePct(rawSoil, &soilPct) != E_OK)
    {
        return CONTROL_ERROR;
    }

    /*
     * Read current pump state.
     */
    if (ACT_Get(ACTUATOR_PUMP, &pumpState) != E_OK)
    {
        return CONTROL_ERROR;
    }

    /*
     * Once timeout happened, keep pump OFF
     * until soil reaches the normal OFF threshold.
     */
    if (g_pumpTimeoutLatched != 0U)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);

        if (soilPct > g_pConfig->soilOffPct)
        {
            g_pumpTimeoutLatched = 0U;
            g_pumpRunTicks = 0U;
        }

        return CONTROL_OK;
    }

    /*
     * Normal irrigation hysteresis.
     */
    if (soilPct < g_pConfig->soilOnPct)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON);
    }
    else if (soilPct > g_pConfig->soilOffPct)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
    }

    /*
     * Read the actual state after hysteresis control.
     */
    if (ACT_Get(ACTUATOR_PUMP, &pumpState) != E_OK)
    {
        return CONTROL_ERROR;
    }

    /*
     * Count continuous pump runtime.
     */
    if (pumpState == ACT_STATE_ON)
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

CONTROL_Status_t CTRL_UpdatePhoto(void)
{
    uint8 Local_u8LightPct;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_GetLight(&Local_u8LightPct) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Local_u8LightPct < g_pConfig->lightOnPct)
    {
        if (ACT_Set(ACTUATOR_LAMP, ACT_STATE_ON) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }
    else if (Local_u8LightPct > g_pConfig->lightOffPct)
    {
        if (ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF) != E_OK)
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