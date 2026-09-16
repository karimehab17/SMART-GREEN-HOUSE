#ifndef CONFIG_H
#define CONFIG_H

#include"STD_TYPES.h"
#include "../MCAL/ADC/ADC_interface.h"
#include "GPIO_interface.h"

/* ================= Actuator Configuration ================= */

#define ACTUATOR_FAN_PORT       GPIO_PORTB
#define ACTUATOR_FAN_PIN        GPIO_PIN0

#define ACTUATOR_PUMP_PORT      GPIO_PORTB
#define ACTUATOR_PUMP_PIN       GPIO_PIN1

#define ACTUATOR_LAMP_PORT      GPIO_PORTB
#define ACTUATOR_LAMP_PIN       GPIO_PIN2

#define ACTUATOR_ALARM_PORT     GPIO_PORTB
#define ACTUATOR_ALARM_PIN      GPIO_PIN3

#define ACTUATOR_BUZZER_PORT    GPIO_PORTD
#define ACTUATOR_BUZZER_PIN     GPIO_PIN7

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


/* =========================================================
 * Scheduler Configuration
 * ========================================================= */

#define SCH_TASK_COUNT              7u
#define SCH_TICK_MS                 10u

/* Task IDs according to project README */
#define SCH_TASK_BUTTONS            0u
#define SCH_TASK_FSM                1u
#define SCH_TASK_SENSORS            2u
#define SCH_TASK_CONTROL            3u
#define SCH_TASK_LCD                4u
#define SCH_TASK_REPORT             5u
#define SCH_TASK_CONSOLE            6u

/* Task periods */
#define SCH_BUTTONS_PERIOD_MS       10u
#define SCH_FSM_PERIOD_MS           10u
#define SCH_SENSORS_PERIOD_MS       100u
#define SCH_CONTROL_PERIOD_MS       200u
#define SCH_LCD_PERIOD_MS           500u
#define SCH_REPORT_PERIOD_MS        5000u
#define SCH_CONSOLE_PERIOD_MS       20u

/* Timer2 / Buzzer Configuration */
#define BUZZER_PORT                 GPIO_PORTD
#define BUZZER_PIN                  GPIO_PIN7

#define BUZZER_PWM_DUTY_PERCENT     50u
#define BUZZER_TONE_HZ              2000u

#endif /* CONFIG_H */