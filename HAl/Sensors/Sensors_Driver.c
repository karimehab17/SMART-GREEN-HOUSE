#include "Sensors_Driver.h"
#include "Math.h"
#include "../../MCAL/ADC/ADC_interface.h"
#include "config.h"
#include <stddef.h>

STD_ReturnType Sensors_Init(void)
{
    return ADC_Init(SENSOR_ADC_REFERENCE, SENSOR_ADC_PRESCALER);
}

STD_ReturnType Sensors_ReadRaw(uint16 *tempRaw,uint16 *soilRaw,uint16 *lightRaw)
{
    if ((tempRaw == NULL) ||
        (soilRaw == NULL) ||
        (lightRaw == NULL))
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(SENSOR_TEMP_CHANNEL, tempRaw) != E_OK)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(SENSOR_SOIL_CHANNEL, soilRaw) != E_OK)
    {
        return E_NOK;
    }

    if (ADC_ReadChannelBlocking(SENSOR_LIGHT_CHANNEL, lightRaw) != E_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

STD_ReturnType Sensors_ScaleTempC(uint16 raw, uint8 *tempC)
{
    if (tempC == NULL)
    {
        return E_NOK;
    }

    uint32 scaled = ((uint32)raw * 50UL + 511UL) / 1023UL;

    *tempC = (uint8)scaled;

    return E_OK;
}

STD_ReturnType Sensors_ScalePct(uint16 raw, uint8 *percent)
{
    if (percent == NULL)
    {
        return E_NOK;
    }

    uint32 scaled = ((uint32)raw * 100UL + 511UL) / 1023UL;

    *percent = (uint8)scaled;

    return E_OK;
}