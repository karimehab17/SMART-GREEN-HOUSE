#include "STD_TYPES.h"
#include "config.h"
#include <stddef.h>

/* ==================== Application ==================== */

#include "scheduler.h"
#include "greenhouse_fsm.h"
#include "control.h"
#include "console.h"
#include "report.h"

/* ==================== HAL ==================== */

#include "Sensors_Driver.h"
#include "Actuators_Driver.h"
#include "Buttons_Driver.h"
#include "lcd_i2c.h"

/* ==================== MCAL ==================== */

#include "TIMER_interface.h"
#include "UART_interface.h"

#include <avr/interrupt.h>

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
    /*
     * Control is executed only in AUTO mode.
     * FSM remains responsible for state transitions
     * and alarm handling.
     */
    if (GHSM_GetState() == ST_AUTO)
    {
        (void)CTRL_UpdateThermal();
        (void)CTRL_UpdateIrrigation();
        (void)CTRL_UpdatePhoto();
    }
}

static void APP_LcdTask(void)
{
    /*
     * LCD presentation is delegated to the LCD/HAL layer
     * through its public API.
     *
     * Detailed screen management should not live in main().
     */
}

static void APP_ReportTask(void)
{
    /*
     * Reporting is owned by the Report application module.
     */
    (void)RPT_Update();
}

static void APP_ConsoleTask(void)
{
    /*
     * UART command processing is owned by Console.
     */
    (void)CON_Process();
}

/* =========================================================
 *                  Hardware Initialization
 * ========================================================= */

static void APP_InitHardware(void)
{
    (void)ACT_Init();

    (void)BTN_Init();

    (void)Sensors_Init();

    (void)UART_Init(UART_BAUD_RATE);

    LCD_Init();
}

/* =========================================================
 *                 Application Initialization
 * ========================================================= */

static void APP_InitApplication(void)
{
    /*
     * Configuration is owned by config.h and Config_t.
     * Runtime configuration loading/default handling
     * is owned by the FSM/configuration logic.
     */

    (void)GHSM_Init(NULL);

    (void)CTRL_Init(NULL);

    (void)RPT_Init(NULL);

    (void)CON_Init(NULL);
}

/* =========================================================
 *                    Scheduler Configuration
 * ========================================================= */

static void APP_CreateTasks(void)
{
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
}

/* =========================================================
 *                     Timer0 Configuration
 * ========================================================= */

static void APP_InitSchedulerTimer(void)
{
    /*
     * Timer0 configuration belongs to the scheduler/timer
     * integration layer.
     */
    (void)TIMER0_Init(TIMER0_CTC);

    (void)TIMER0_SetCompareValue(77U);

    (void)TIMER0_SetCompareInterrupt(
        TIMER_INTERRUPT_ENABLE
    );

    (void)TIMER0_Start(
        TIMER0_PRESC_1024
    );
}

/* =========================================================
 *                           MAIN
 * ========================================================= */

int main(void)
{
    APP_InitHardware();

    (void)SCH_Init();

    /*
     * Application initialization.
     */
    /*
     * NOTE:
     * GHSM_Init / CTRL_Init / RPT_Init / CON_Init currently
     * require runtime pointers in their existing APIs.
     * These dependencies must be resolved by the application
     * modules before final integration.
     */

    APP_CreateTasks();

    APP_InitSchedulerTimer();

    (void)UART_SetRxInterrupt(
        UART_INTERRUPT_ENABLE
    );

    /*
     * Enable global interrupts only after all peripheral
     * and scheduler configuration is complete.
     */
    sei();

    /*
     * Cooperative super loop.
     * No blocking work is performed here.
     */
    while (1)
    {
        SCH_Run();
    }

    return 0;
}