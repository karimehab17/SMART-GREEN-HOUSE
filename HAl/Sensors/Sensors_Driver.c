#include "Sensors_Driver.h"
#include "../../MCAL/ADC/ADC_interface.h"
#include "config.h"
#include "report.h"
#include <stddef.h>

static uint16 g_tempRaw = 0U;
static uint16 g_soilRaw = 0U;
static uint16 g_lightRaw = 0U;

static uint8 g_tempC = 0U;
static uint8 g_soilPct = 0U;
static uint8 g_lightPct = 0U;

#define SENSOR_FAULT_TICKS 50U

static uint8 g_sensorFaultTicks[3] = {0U, 0U, 0U};
static uint8 g_sensorFault[3] = {0U, 0U, 0U};


static uint16 Sensors_Median3(
    uint16 a,
    uint16 b,
    uint16 c)
{
    if (a > b)
    {
        if (b > c)
        {
            return b;
        }

        if (a > c)
        {
            return c;
        }

        return a;
    }

    if (a > c)
    {
        return a;
    }

    if (b > c)
    {
        return c;
    }

    return b;
}


static STD_ReturnType Sensors_ReadFilteredChannel(
    uint8 channel,
    uint16 *result)
{
    uint16 sample1;
    uint16 sample2;
    uint16 sample3;

    if (result == NULL)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(channel, &sample1) != E_OK)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(channel, &sample2) != E_OK)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(channel, &sample3) != E_OK)
    {
        return E_NOK;
    }

    *result = Sensors_Median3(sample1, sample2, sample3);

    return E_OK;
}


static uint8 Sensors_IsInvalidRaw(uint16 raw)
{
    return (uint8)((raw == 0U) || (raw >= ADC_MAX_VALUE));
}


static void Sensors_UpdateFault(
    SensorId_t sensor,
    uint16 raw)
{
    if (Sensors_IsInvalidRaw(raw) != 0U)
    {
        if (g_sensorFaultTicks[sensor] < SENSOR_FAULT_TICKS)
        {
            g_sensorFaultTicks[sensor]++;
        }

        if ((g_sensorFaultTicks[sensor] >= SENSOR_FAULT_TICKS) &&
            (g_sensorFault[sensor] == 0U))
        {
            g_sensorFault[sensor] = 1U;

            if (sensor == SEN_TEMP)
            {
                (void)RPT_SendEvent(
                    (const uint8 *)"SENSOR FAULT 1");
            }
            else if (sensor == SEN_SOIL)
            {
                (void)RPT_SendEvent(
                    (const uint8 *)"SENSOR FAULT 2");
            }
            else
            {
                (void)RPT_SendEvent(
                    (const uint8 *)"SENSOR FAULT 3");
            }
        }
    }
    else
    {
        g_sensorFaultTicks[sensor] = 0U;
        g_sensorFault[sensor] = 0U;
    }
}


static void Sensors_UpdateFaults(
    uint16 tempRaw,
    uint16 soilRaw,
    uint16 lightRaw)
{
    Sensors_UpdateFault(SEN_TEMP, tempRaw);
    Sensors_UpdateFault(SEN_SOIL, soilRaw);
    Sensors_UpdateFault(SEN_LIGHT, lightRaw);
}


uint8 Sensors_HasSensorFault(void)
{
    return (uint8)(
        (g_sensorFault[SEN_TEMP] != 0U) ||
        (g_sensorFault[SEN_SOIL] != 0U) ||
        (g_sensorFault[SEN_LIGHT] != 0U));
}


STD_ReturnType Sensors_Init(void)
{
    g_tempRaw = 0U;
    g_soilRaw = 0U;
    g_lightRaw = 0U;

    g_tempC = 0U;
    g_soilPct = 0U;
    g_lightPct = 0U;

    g_sensorFaultTicks[SEN_TEMP] = 0U;
    g_sensorFaultTicks[SEN_SOIL] = 0U;
    g_sensorFaultTicks[SEN_LIGHT] = 0U;

    g_sensorFault[SEN_TEMP] = 0U;
    g_sensorFault[SEN_SOIL] = 0U;
    g_sensorFault[SEN_LIGHT] = 0U;

    return ADC_Init(
        SENSOR_ADC_REFERENCE,
        SENSOR_ADC_PRESCALER);
}


STD_ReturnType Sensors_Update(void)
{
    uint16 tempRaw;
    uint16 soilRaw;
    uint16 lightRaw;

    if (Sensors_ReadFilteredChannel(
            SENSOR_TEMP_CHANNEL,
            &tempRaw) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_ReadFilteredChannel(
            SENSOR_SOIL_CHANNEL,
            &soilRaw) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_ReadFilteredChannel(
            SENSOR_LIGHT_CHANNEL,
            &lightRaw) != E_OK)
    {
        return E_NOK;
    }

    Sensors_UpdateFaults(
        tempRaw,
        soilRaw,
        lightRaw);

    if (Sensors_ScaleTempC(
            tempRaw,
            &g_tempC) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_ScalePct(
            soilRaw,
            &g_soilPct) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_ScalePct(
            lightRaw,
            &g_lightPct) != E_OK)
    {
        return E_NOK;
    }

    g_tempRaw = tempRaw;
    g_soilRaw = soilRaw;
    g_lightRaw = lightRaw;

    return E_OK;
}


STD_ReturnType Sensors_ReadRaw(
    uint16 *tempRaw,
    uint16 *soilRaw,
    uint16 *lightRaw)
{
    if ((tempRaw == NULL) ||
        (soilRaw == NULL) ||
        (lightRaw == NULL))
    {
        return E_NOK;
    }

    *tempRaw = g_tempRaw;
    *soilRaw = g_soilRaw;
    *lightRaw = g_lightRaw;

    return E_OK;
}


STD_ReturnType Sensors_GetTemperature(
    uint8 *tempC)
{
    if (tempC == NULL)
    {
        return E_NOK;
    }

    *tempC = g_tempC;

    return E_OK;
}


STD_ReturnType Sensors_GetSoil(
    uint8 *soilPct)
{
    if (soilPct == NULL)
    {
        return E_NOK;
    }

    *soilPct = g_soilPct;

    return E_OK;
}


STD_ReturnType Sensors_GetLight(
    uint8 *lightPct)
{
    if (lightPct == NULL)
    {
        return E_NOK;
    }

    *lightPct = g_lightPct;

    return E_OK;
}


STD_ReturnType Sensors_ScaleTempC(
    uint16 raw,
    uint8 *tempC)
{
    if (tempC == NULL)
    {
        return E_NOK;
    }

    if (raw > ADC_MAX_VALUE)
    {
        return E_NOK;
    }

    *tempC = TEMP_SCALE(raw);

    return E_OK;
}


STD_ReturnType Sensors_ScalePct(
    uint16 raw,
    uint8 *percent)
{
    if (percent == NULL)
    {
        return E_NOK;
    }

    if (raw > ADC_MAX_VALUE)
    {
        return E_NOK;
    }

    *percent = PCT_SCALE(raw);

    return E_OK;
}