#include "config.h"

#include "scheduler.h"
#include "control.h"
#include "greenhouse_fsm.h"
#include "report.h"
#include "console.h"

#include "Buttons_Driver.h"
#include "lcd_i2c.h"
#include "Sensors_Driver.h"
#include "Actuators_Driver.h"

#include "UART_interface.h"
#include "TIMER_interface.h"
#include "INTERRUPT_interface.h"


static Config_t g_config;
static SysData_t g_sysData;


static void Task_Buttons(void)
{
    (void)BTN_Poll();
}

static void Task_Sensors(void)
{
    (void)Sensors_Update();
}

static void Task_Control(void)
{
    (void)CTRL_Update();
}

static void Task_FSM(void)
{
    (void)GHSM_Update();
}

static void Task_LCD(void)
{
    uint8 temp;
    uint8 soil;
    uint8 light;

    Sensors_GetTemperature(&temp);
    Sensors_GetSoil(&soil);
    Sensors_GetLight(&light);

    LCD_Clear();

    LCD_Goto(0U, 0U);
    LCD_Print("T:");
    LCD_PrintNum(temp);
    LCD_Print("C S:");
    LCD_PrintNum(soil);
    LCD_Print("%");

    LCD_Goto(1U, 0U);
    LCD_Print("L:");
    LCD_PrintNum(light);
    LCD_Print("% M:");

    if (GHSM_GetState() == ST_AUTO)
        LCD_Print("AUTO");
    else if (GHSM_GetState() == ST_MANUAL)
        LCD_Print("MANUAL");
    else if (GHSM_GetState() == ST_ALARM)
        LCD_Print("ALARM");
    else
        LCD_Print("CONFIG");
}

static void Task_Report(void)
{
    (void)RPT_Update();
}

static void Task_Console(void)
{
    (void)CON_Process();
}


int main(void)
{
    UART_Init(UART_BAUD_RATE);
    BTN_Init();
    LCD_Init();

    GHSM_Init(&g_config);
    CTRL_Init(&g_config);
    RPT_Init(&g_sysData);
    CON_Init(&g_config);

    SCH_Init();

    SCH_CreateTask(
        SCH_TASK_BUTTONS,
        Task_Buttons,
        SCH_BUTTONS_PERIOD_MS);

    SCH_CreateTask(
        SCH_TASK_SENSORS,
        Task_Sensors,
        SCH_SENSORS_PERIOD_MS);

    SCH_CreateTask(
        SCH_TASK_CONTROL,
        Task_Control,
        SCH_CONTROL_PERIOD_MS);

    SCH_CreateTask(
        SCH_TASK_FSM,
        Task_FSM,
        SCH_FSM_PERIOD_MS);

    SCH_CreateTask(
        SCH_TASK_LCD,
        Task_LCD,
        SCH_LCD_PERIOD_MS);

    SCH_CreateTask(
        SCH_TASK_REPORT,
        Task_Report,
        SCH_REPORT_PERIOD_MS);

    SCH_CreateTask(
        SCH_TASK_CONSOLE,
        Task_Console,
        SCH_CONSOLE_PERIOD_MS);

    TIMER0_Init(TIMER0_CTC);
    TIMER0_SetCompareValue(77U);
    TIMER0_SetCompareInterrupt(
        TIMER_INTERRUPT_ENABLE);
    TIMER0_Start(TIMER0_PRESC_1024);

    UART_SetRxInterrupt(
        UART_INTERRUPT_ENABLE);

    INTERRUPT_EnableGlobal();

    while (1)
    {
        SCH_Run();
    }
}