#ifndef CONFIG_H
#define CONFIG_H

#include "../MCAL/ADC/ADC_interface.h"

/* ================= Sensor Configuration ================= */

#define SENSOR_TEMP_CHANNEL   ADC_CHANNEL_0
#define SENSOR_SOIL_CHANNEL   ADC_CHANNEL_1
#define SENSOR_LIGHT_CHANNEL  ADC_CHANNEL_2

#define SENSOR_ADC_REFERENCE  ADC_REF_AVCC
#define SENSOR_ADC_PRESCALER  ADC_PRESC_64


/* ================= Runtime Configuration ================= */

#define CFG_MAGIC             0xA5C3u
#define CFG_VERSION           0x01u

#define DEFAULT_TEMP_ON_C     35u
#define DEFAULT_TEMP_OFF_C    32u

#define DEFAULT_SOIL_ON_PCT   40u
#define DEFAULT_SOIL_OFF_PCT  60u

#define DEFAULT_LIGHT_ON_PCT  25u
#define DEFAULT_LIGHT_OFF_PCT 40u

#define DEFAULT_TEMP_ALARM_C  45u
#define DEFAULT_SOIL_ALARM_PCT 15u

#define DEFAULT_MODE          0u


typedef struct
{
    uint16 magic;
    uint8  version;

    uint8  tempOnC;
    uint8  tempOffC;

    uint8  soilOnPct;
    uint8  soilOffPct;

    uint8  lightOnPct;
    uint8  lightOffPct;

    uint8  tempAlarmC;
    uint8  soilAlarmPct;

    uint8  mode;
    uint8  checksum;

} Config_t;

#endif /* CONFIG_H */