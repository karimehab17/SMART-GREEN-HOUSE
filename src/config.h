#ifndef CONFIG_H
#define CONFIG_H

#include"STD_TYPES.h"
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

/* ================= LCD Configuration ================= */

#define LCD_I2C_ADDRESS        0x3Eu

#define LCD_COMMAND_CONTROL    0x00u
#define LCD_DATA_CONTROL       0x40u

#define LCD_FUNCTION_SET       0x38u
#define LCD_DISPLAY_ON         0x0Cu
#define LCD_CLEAR_DISPLAY      0x01u
#define LCD_ENTRY_MODE         0x06u

#define LCD_LINE0_ADDRESS      0x80u
#define LCD_LINE1_ADDRESS      0xC0u

#define LCD_I2C_FREQUENCY      100000UL

#define LCD_INIT_DELAY_MS      50u
#define LCD_WRITE_DELAY_MS     2u
#define LCD_CLEAR_DELAY_MS     2u
} Config_t;

#endif /* CONFIG_H */