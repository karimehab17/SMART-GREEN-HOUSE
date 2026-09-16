#define F_CPU 8000000UL

#include "STD_TYPES.h"

#include "scheduler.h"
#include "console.h"
#include "control.h"
#include "greenhouse_fsm.h"
#include "report.h"

#include "Sensors_Driver.h"
#include "Actuators_Driver.h"
#include "Buttons_Driver.h"
#include "lcd_i2c.h"

#include "timer_interface.h"
#include "uart_interface.h"

#include <avr/interrupt.h>

/* =========================================================
 *                    Global Application Data
 * ========================================================= */

static Config_t g_config;
static SysData_t g_sysData;

static uint8 g_halfSecondTicks = 0U;


/* =========================================================
 *                    Configuration
 * ========================================================= */

static void APP_LoadDefaults(void)
{
    g_config.magic = CFG_MAGIC;
    g_config.version = CFG_VERSION;

    g_config.tempOnC = DEFAULT_TEMP_ON_C;
    g_config.tempOffC = DEFAULT_TEMP_OFF_C;

    g_config.soilOnPct = DEFAULT_SOIL_ON_PCT;
    g_config.soilOffPct = DEFAULT_SOIL_OFF_PCT;

    g_config.lightOnPct = DEFAULT_LIGHT_ON_PCT;
    g_config.lightOffPct = DEFAULT_LIGHT_OFF_PCT;

    g_config.tempAlarmC = DEFAULT_TEMP_ALARM_C;
    g_config.soilAlarmPct = DEFAULT_SOIL_ALARM_PCT;

    g_config.mode = DEFAULT_MODE;
    g_config.checksum = 0U;
}


/* =========================================================
 *                    System Data Sync
 * ========================================================= */

static void APP_SyncSystemData(void)
{
    uint16 tempRaw = 0U;
    uint16 soilRaw = 0U;
    uint16 lightRaw = 0U;

    ActuatorStateType state = ACT_STATE_OFF;

    (void)Sensors_ReadRaw(
        &tempRaw,
        &soilRaw,
        &lightRaw
    );

    (void)Sensors_GetTemperature(
        &g_sysData.tempC
    );

    (void)Sensors_GetSoil(
        &g_sysData.soilPct
    );

    (void)Sensors_GetLight(
        &g_sysData.lightPct
    );

    g_sysData.adcRaw[0] = tempRaw;
    g_sysData.adcRaw[1] = soilRaw;
    g_sysData.adcRaw[2] = lightRaw;

    (void)ACT_Get(
        ACTUATOR_FAN,
        &state
    );

    g_sysData.fanOn = (uint8)state;

    (void)ACT_Get(
        ACTUATOR_PUMP,
        &state
    );

    g_sysData.pumpOn = (uint8)state;

    (void)ACT_Get(
        ACTUATOR_LAMP,
        &state
    );

    g_sysData.lampOn = (uint8)state;

    (void)ACT_Get(
        ACTUATOR_ALARM,
        &state
    );

    g_sysData.alarmOn = (uint8)state;

    g_sysData.mode =
        (uint8)GHSM_GetState();
}


/* =========================================================
 *                    Scheduler Tasks
 * ========================================================= */

static void APP_ButtonsTask(void)
{
    (void)BTN_Poll();
}


static void APP_FsmTask(void)
{
    (void)GHSM_Update();
}


static void APP_SensorsTask(void)
{
    (void)Sensors_Update();
}


static void APP_ControlTask(void)
{
    if (GHSM_GetState() == ST_AUTO)
    {
        (void)CTRL_Update();
    }
}


static void APP_LcdTask(void)
{
    APP_SyncSystemData();

    g_halfSecondTicks++;

    if (g_halfSecondTicks >= 2U)
    {
        g_halfSecondTicks = 0U;

        if (g_sysData.upTimeSec < 65535U)
        {
            g_sysData.upTimeSec++;
        }
    }

    LCD_Goto(0U, 0U);

    LCD_Print("T");
    LCD_PrintNum(g_sysData.tempC);

    LCD_Print(" S");
    LCD_PrintNum(g_sysData.soilPct);

    LCD_Print(" L");
    LCD_PrintNum(g_sysData.lightPct);

    LCD_Print("   ");

    LCD_Goto(1U, 0U);

    LCD_Print("F");
    LCD_PrintNum(g_sysData.fanOn);

    LCD_Print(" P");
    LCD_PrintNum(g_sysData.pumpOn);

    LCD_Print(" L");
    LCD_PrintNum(g_sysData.lampOn);

    LCD_Print(" ");

    if (g_sysData.mode == ST_MANUAL)
    {
        LCD_Print("MANUAL ");
    }
    else if (g_sysData.mode == ST_ALARM)
    {
        LCD_Print("ALARM  ");
    }
    else
    {
        LCD_Print("AUTO   ");
    }
}


static void APP_ReportTask(void)
{
    APP_SyncSystemData();

    (void)RPT_Update();
}


static void APP_ConsoleTask(void)
{
    (void)CON_Process();
}



/* =========================================================
 *                         MAIN
 * ========================================================= */

int main(void)
{
    /* ---------------- Configuration ---------------- */

    APP_LoadDefaults();

    /* ---------------- HAL Initialization ---------------- */

    (void)ACT_Init();

    (void)BTN_Init();

    (void)Sensors_Init();

    /* ---------------- Communication ---------------- */

    (void)UART_Init(UART_BAUD_RATE);

    LCD_Init();

    /* ---------------- Application Initialization ---------------- */

    (void)GHSM_Init(&g_config);

    (void)CTRL_Init(&g_config);

    (void)RPT_Init(&g_sysData);

    (void)CON_Init(&g_config);

    (void)SCH_Init();

    /* ---------------- Runtime Data ---------------- */

    g_sysData.upTimeSec = 0U;

    APP_SyncSystemData();

    LCD_Clear();

    /* =================================================
     *                    Scheduler Tasks
     * ================================================= */

    (void)SCH_CreateTask(
        SCH_TASK_BUTTONS,
        APP_ButtonsTask,
        SCH_BUTTONS_PERIOD_MS
    );

    (void)SCH_CreateTask(
        SCH_TASK_FSM,
        APP_FsmTask,
        SCH_FSM_PERIOD_MS
    );

    (void)SCH_CreateTask(
        SCH_TASK_SENSORS,
        APP_SensorsTask,
        SCH_SENSORS_PERIOD_MS
    );

    (void)SCH_CreateTask(
        SCH_TASK_CONTROL,
        APP_ControlTask,
        SCH_CONTROL_PERIOD_MS
    );

    (void)SCH_CreateTask(
        SCH_TASK_LCD,
        APP_LcdTask,
        SCH_LCD_PERIOD_MS
    );

    (void)SCH_CreateTask(
        SCH_TASK_REPORT,
        APP_ReportTask,
        SCH_REPORT_PERIOD_MS
    );

    (void)SCH_CreateTask(
        SCH_TASK_CONSOLE,
        APP_ConsoleTask,
        SCH_CONSOLE_PERIOD_MS
    );

    /* =================================================
     *                    Timer0 Setup
     * ================================================= */

    (void)TIMER0_Init(TIMER0_CTC);

    /*
     * F_CPU = 8 MHz
     * Prescaler = 1024
     * OCR0 = 77
     *
     * Tick ~= 10 ms
     */

    (void)TIMER0_SetCompareValue(77U);

    (void)TIMER0_SetCompareInterrupt(
        TIMER_INTERRUPT_ENABLE
    );

    (void)TIMER0_Start(
        TIMER0_PRESC_1024
    );

    /* ---------------- UART RX Interrupt ---------------- */

    (void)UART_SetRxInterrupt(
        UART_INTERRUPT_ENABLE
    );

    /* ---------------- Global Interrupt ---------------- */

    sei();

    /* =================================================
     *                    Super Loop
     * ================================================= */

    while (1)
    {
        SCH_Run();
    }

    return 0;
}