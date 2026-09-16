#include "Sensors_Driver.h"

#include "../../MCAL/ADC/ADC_interface.h"
#include "config.h"

#include <stddef.h>

static uint16 g_tempRaw = 0U;
static uint16 g_soilRaw = 0U;
static uint16 g_lightRaw = 0U;

static uint16 Sensors_Median3(uint16 a, uint16 b, uint16 c)
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

static STD_ReturnType Sensors_ReadFilteredChannel(uint8 channel,
                                                   uint16 *result)
{
    uint16 sample1 = 0U;
    uint16 sample2 = 0U;
    uint16 sample3 = 0U;

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

STD_ReturnType Sensors_Init(void)
{
    g_tempRaw = 0U;
    g_soilRaw = 0U;
    g_lightRaw = 0U;

    return ADC_Init(SENSOR_ADC_REFERENCE,
                   SENSOR_ADC_PRESCALER);
}

STD_ReturnType Sensors_Update(void)
{
    uint16 tempRaw = 0U;
    uint16 soilRaw = 0U;
    uint16 lightRaw = 0U;

    if (Sensors_ReadFilteredChannel(SENSOR_TEMP_CHANNEL,
                                    &tempRaw) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_ReadFilteredChannel(SENSOR_SOIL_CHANNEL,
                                    &soilRaw) != E_OK)
    {
        return E_NOK;
    }

    if (Sensors_ReadFilteredChannel(SENSOR_LIGHT_CHANNEL,
                                    &lightRaw) != E_OK)
    {
        return E_NOK;
    }

    g_tempRaw = tempRaw;
    g_soilRaw = soilRaw;
    g_lightRaw = lightRaw;

    return E_OK;
}

STD_ReturnType Sensors_ReadRaw(uint16 *tempRaw,
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

STD_ReturnType Sensors_ScaleTempC(uint16 raw, uint8 *tempC)
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

STD_ReturnType Sensors_ScalePct(uint16 raw, uint8 *percent)
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