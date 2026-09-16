#include "control.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include <stddef.h>

static Config_t *g_pConfig = NULL;

CONTROL_Status_t CTRL_Init(Config_t *pConfig)
{
    if (pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    g_pConfig = pConfig;

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
    uint8 Local_u8SoilPct;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_GetSoil(&Local_u8SoilPct) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Local_u8SoilPct < g_pConfig->soilOnPct)
    {
        if (ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON) != E_OK)
        {
            return CONTROL_ERROR;
        }
    }
    else if (Local_u8SoilPct > g_pConfig->soilOffPct)
    {
        if (ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF) != E_OK)
        {
            return CONTROL_ERROR;
        }
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