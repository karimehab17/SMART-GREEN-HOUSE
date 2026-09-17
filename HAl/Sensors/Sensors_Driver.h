#ifndef SENSORS_DRIVER_H
#define SENSORS_DRIVER_H

#include "config.h"
#include "../../LIB/STD_TYPES.h"

STD_ReturnType Sensors_Init(void);

STD_ReturnType Sensors_Update(void);

STD_ReturnType Sensors_ReadRaw(
    uint16 *tempRaw,
    uint16 *soilRaw,
    uint16 *lightRaw);

STD_ReturnType Sensors_GetTemperature(uint8 *tempC);
STD_ReturnType Sensors_GetSoil(uint8 *soilPct);
STD_ReturnType Sensors_GetLight(uint8 *lightPct);

uint8 Sensors_HasSensorFault(void);

STD_ReturnType Sensors_ScaleTempC(
    uint16 raw,
    uint8 *tempC);

STD_ReturnType Sensors_ScalePct(
    uint16 raw,
    uint8 *percent);

#endif /* SENSORS_DRIVER_H */