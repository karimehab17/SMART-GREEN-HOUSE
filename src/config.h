#ifndef CONFIG_H
#define CONFIG_H

#include "STD_TYPES.h"

/* =========================================================
 * System Configuration
 * ========================================================= */

#define F_CPU                       8000000UL


/* =========================================================
 * GPIO Configuration
 * ========================================================= */

/* ---------------- Sensors ---------------- */

#define SENSOR_TEMP_PORT            GPIO_PORTA
#define SENSOR_TEMP_PIN             GPIO_PIN0
#define SENSOR_TEMP_CHANNEL         0U

#define SENSOR_SOIL_PORT            GPIO_PORTA
#define SENSOR_SOIL_PIN             GPIO_PIN1
#define SENSOR_SOIL_CHANNEL         1U

#define SENSOR_LIGHT_PORT           GPIO_PORTA
#define SENSOR_LIGHT_PIN            GPIO_PIN2
#define SENSOR_LIGHT_CHANNEL        2U


/* ---------------- Actuators ---------------- */

#define ACTUATOR_FAN_PORT           GPIO_PORTB
#define ACTUATOR_FAN_PIN            GPIO_PIN0

#define ACTUATOR_PUMP_PORT          GPIO_PORTB
#define ACTUATOR_PUMP_PIN           GPIO_PIN1

#define ACTUATOR_LAMP_PORT          GPIO_PORTB
#define ACTUATOR_LAMP_PIN           GPIO_PIN2

#define ACTUATOR_ALARM_PORT         GPIO_PORTB
#define ACTUATOR_ALARM_PIN          GPIO_PIN3

#define ACTUATOR_BUZZER_PORT        GPIO_PORTD
#define ACTUATOR_BUZZER_PIN         GPIO_PIN7


/* ---------------- Buttons ---------------- */

#define BUTTON_RESET_PORT           GPIO_PORTD
#define BUTTON_RESET_PIN            GPIO_PIN2

#define BUTTON_MODE_PORT            GPIO_PORTD
#define BUTTON_MODE_PIN             GPIO_PIN3

#define BUTTON_SAVE_PORT            GPIO_PORTD
#define BUTTON_SAVE_PIN             GPIO_PIN4


/* =========================================================
 * ADC Configuration
 * ========================================================= */

#define ADC_REFERENCE_AVCC          0U
#define ADC_PRESCALER_64            6U

#define SENSOR_ADC_REFERENCE        ADC_REFERENCE_AVCC
#define SENSOR_ADC_PRESCALER        ADC_PRESCALER_64

#define ADC_MAX_VALUE               1023U

#define SENSOR_FILTER_SAMPLES       3U
#define SENSOR_CHANNEL_COUNT        3U


/* =========================================================
 * Sensor Scaling
 * ========================================================= */

#define TEMP_MIN_C                  0U
#define TEMP_MAX_C                  50U

#define PERCENT_MIN                 0U
#define PERCENT_MAX                 100U


/* =========================================================
 * Scheduler Configuration
 * ========================================================= */

#define SCH_TICK_MS                 10U

#define SCH_TASK_COUNT              7U

#define SCH_TASK_BUTTONS            0U
#define SCH_TASK_FSM                1U
#define SCH_TASK_SENSORS            2U
#define SCH_TASK_CONTROL            3U
#define SCH_TASK_LCD                4U
#define SCH_TASK_REPORT             5U
#define SCH_TASK_CONSOLE            6U

#define SCH_BUTTONS_PERIOD_MS       10U
#define SCH_FSM_PERIOD_MS           10U
#define SCH_SENSORS_PERIOD_MS       100U
#define SCH_CONTROL_PERIOD_MS       200U
#define SCH_LCD_PERIOD_MS           500U
#define SCH_REPORT_PERIOD_MS        5000U
#define SCH_CONSOLE_PERIOD_MS       20U


/* =========================================================
 * Default Runtime Configuration
 * ========================================================= */

#define CFG_MAGIC                   0xA5C3U
#define CFG_VERSION                 0x01U

#define DEFAULT_TEMP_ON_C           35U
#define DEFAULT_TEMP_OFF_C          32U

#define DEFAULT_SOIL_ON_PCT         40U
#define DEFAULT_SOIL_OFF_PCT        60U

#define DEFAULT_LIGHT_ON_PCT        25U
#define DEFAULT_LIGHT_OFF_PCT       40U

#define DEFAULT_TEMP_ALARM_C        45U
#define DEFAULT_SOIL_ALARM_PCT      15U

#define DEFAULT_MODE                0U


/* =========================================================
 * Hysteresis Configuration
 * ========================================================= */

#define FAN_ON_THRESHOLD_C          35U
#define FAN_OFF_THRESHOLD_C         32U

#define PUMP_ON_THRESHOLD_PCT       40U
#define PUMP_OFF_THRESHOLD_PCT      60U

#define LAMP_ON_THRESHOLD_PCT       25U
#define LAMP_OFF_THRESHOLD_PCT      40U


/* =========================================================
 * Alarm Configuration
 * ========================================================= */

#define ALARM_TEMP_DEFAULT_C        45U
#define ALARM_SOIL_DEFAULT_PCT      15U

#define ALARM_BUZZER_ON_MS          100U
#define ALARM_BUZZER_OFF_MS         900U

#define SCH_ALARM_ON_TICKS \
    (ALARM_BUZZER_ON_MS / SCH_TICK_MS)

#define SCH_ALARM_PERIOD_TICKS \
    ((ALARM_BUZZER_ON_MS + ALARM_BUZZER_OFF_MS) / SCH_TICK_MS)


/* =========================================================
 * Button Configuration
 * ========================================================= */

#define BUTTON_DEBOUNCE_MS          20U
#define BUTTON_DEBOUNCE_SAMPLES     2U

#define BUTTON_PRESSED              0U
#define BUTTON_RELEASED             1U

#define FACTORY_RESET_HOLD_MS       3000U
#define FACTORY_RESET_TICKS         300U


/* =========================================================
 * Pump Protection
 * ========================================================= */

#define PUMP_MAX_RUNTIME_SEC        60U


/* =========================================================
 * UART Configuration
 * ========================================================= */

#define UART_BAUD_RATE              9600UL

#define UART_RX_BUFFER_SIZE         64U

#define UART_MAX_COMMAND_LENGTH     24U

#define UART_TX_BUFFER_SIZE         128U

/* =========================================================
 * LCD Configuration
 *
 * LCD communication is I2C/AIP.
 * SPI and 74HC595 are NOT used.
 * ========================================================= */

#define LCD_I2C_ADDRESS             0x3EU

#define LCD_COMMAND_CONTROL         0x00U
#define LCD_DATA_CONTROL            0x40U

#define LCD_FUNCTION_SET            0x38U
#define LCD_DISPLAY_ON              0x0CU
#define LCD_CLEAR_DISPLAY           0x01U
#define LCD_ENTRY_MODE              0x06U

#define LCD_LINE0_ADDRESS           0x80U
#define LCD_LINE1_ADDRESS           0xC0U

#define LCD_I2C_FREQUENCY           100000UL

#define LCD_INIT_DELAY_MS           50U
#define LCD_CLEAR_DELAY_MS          2U


/* =========================================================
 * Buzzer Configuration
 * ========================================================= */

#define BUZZER_PWM_DUTY_PERCENT     50U
#define BUZZER_TONE_HZ              2000U


/* =========================================================
 * EEPROM Configuration
 *
 * Internal ATmega32A EEPROM is used.
 * No SPI EEPROM is used.
 * ========================================================= */

#define EEPROM_CONFIG_ADDRESS       0x00U


/* =========================================================
 * System States
 * ========================================================= */

typedef enum
{
    ST_INIT = 0U,
    ST_AUTO,
    ST_MANUAL,
    ST_ALARM,
    ST_CONFIG

} SystemState_t;


/* =========================================================
 * Actuator Identifiers
 * ========================================================= */

typedef enum
{
    ACTUATOR_FAN = 0U,
    ACTUATOR_PUMP,
    ACTUATOR_LAMP,
    ACTUATOR_ALARM,
    ACTUATOR_BUZZER

} ActuatorId_t;


/* =========================================================
 * Sensor Identifiers
 * ========================================================= */

typedef enum
{
    SEN_TEMP = 0U,
    SEN_SOIL,
    SEN_LIGHT

} SensorId_t;


/* =========================================================
 * System Runtime Data
 * ========================================================= */

typedef struct
{
    uint16 adcRaw[3];

    uint8 tempC;
    uint8 soilPct;
    uint8 lightPct;

    uint8 fanOn;
    uint8 pumpOn;
    uint8 lampOn;
    uint8 alarmOn;

    uint8 mode;

    uint16 upTimeSec;

} SysData_t;


/* =========================================================
 * Persistent Configuration
 *
 * Required size = 13 bytes
 * ========================================================= */

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


/* =========================================================
 * Sensor Scaling Macros
 * ========================================================= */

#define TEMP_SCALE(raw) \
    ((uint8)(((uint32)(raw) * TEMP_MAX_C) / ADC_MAX_VALUE))

#define PCT_SCALE(raw) \
    ((uint8)(((uint32)(raw) * PERCENT_MAX) / ADC_MAX_VALUE))


/* =========================================================
 * Actuator Logic Levels
 * ========================================================= */

#define ACTUATOR_ON                1U
#define ACTUATOR_OFF               0U


/* =========================================================
 * Configuration Validation
 * ========================================================= */

#define CFG_MIN_TEMP_HYSTERESIS_C  2U


#endif /* CONFIG_H */