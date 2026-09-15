#include "control.h"
#include "Actuators_Driver.h"
#include "Sensors_Driver.h"
#include "STD_TYPES.h"
#include <stddef.h>

static Config_t *g_pConfig = NULL;


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

    return CONTROL_OK;
}


/* =========================================================
 * Thermal Control
 * Fan hysteresis:
 * ON  : temperature > tempOnC
 * OFF : temperature < tempOffC
 * ========================================================= */

CONTROL_Status_t CTRL_UpdateThermal(void)
{
    uint16 rawTemp = 0u;
    uint8 tempC = 0u;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_ReadRaw(&rawTemp, NULL, NULL) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_ScaleTempC(rawTemp, &tempC) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (tempC > g_pConfig->tempOnC)
    {
        (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
    }
    else if (tempC < g_pConfig->tempOffC)
    {
        (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);
    }

    return CONTROL_OK;
}


/* =========================================================
 * Irrigation Control
 * Pump hysteresis:
 * ON  : soil < soilOnPct
 * OFF : soil > soilOffPct
 * ========================================================= */

CONTROL_Status_t CTRL_UpdateIrrigation(void)
{
    uint16 rawSoil = 0u;
    uint8 soilPct = 0u;

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

    if (soilPct < g_pConfig->soilOnPct)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON);
    }
    else if (soilPct > g_pConfig->soilOffPct)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
    }

    return CONTROL_OK;
}


/* =========================================================
 * Photo Control
 * Lamp hysteresis:
 * ON  : light < lightOnPct
 * OFF : light > lightOffPct
 * ========================================================= */

CONTROL_Status_t CTRL_UpdatePhoto(void)
{
    uint16 rawLight = 0u;
    uint8 lightPct = 0u;

    if (g_pConfig == NULL)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_ReadRaw(NULL, NULL, &rawLight) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (Sensors_ScalePct(rawLight, &lightPct) != E_OK)
    {
        return CONTROL_ERROR;
    }

    if (lightPct < g_pConfig->lightOnPct)
    {
        (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_ON);
    }
    else if (lightPct > g_pConfig->lightOffPct)
    {
        (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
    }

    return CONTROL_OK;
}