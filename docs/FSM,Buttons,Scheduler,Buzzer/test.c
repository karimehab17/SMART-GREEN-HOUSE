#define F_CPU 8000000UL

#include "STD_TYPES.h"
#include "config.h"

#include "scheduler.h"
#include "Buttons_Driver.h"
#include "Actuators_Driver.h"
#include "TIMER_interface.h"
#include "GPIO_interface.h"

#include <avr/interrupt.h>


/* =========================================================
 * Test State
 * ========================================================= */

/*
 * 0 = AUTO
 * 1 = MANUAL
 */
static uint8 g_manualMode = 0U;

/*
 * 0 = Alarm OFF
 * 1 = Alarm ON
 *
 * Alarm is LATCHED.
 */
static uint8 g_alarmActive = 0U;


static void Test_UpdateLEDs(void)
{
    /* Alarm has highest priority */

    if (g_alarmActive != 0U)
    {
        /* AUTO OFF */
        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN5,
            GPIO_HIGH
        );

        /* MANUAL OFF */
        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN6,
            GPIO_HIGH
        );

        /* ALARM ON */
        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN7,
            GPIO_LOW
        );

        return;
    }


    /* Alarm OFF */

    (void)GPIO_SetPinValue(
        GPIO_PORTB,
        GPIO_PIN7,
        GPIO_HIGH
    );


    /* AUTO */

    if (g_manualMode == 0U)
    {
        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN5,
            GPIO_LOW
        );

        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN6,
            GPIO_HIGH
        );
    }

    /* MANUAL */

    else
    {
        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN5,
            GPIO_HIGH
        );

        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN6,
            GPIO_LOW
        );
    }
}


/* =========================================================
 * Buttons Task
 *
 * Poll buttons every 10 ms
 * ========================================================= */

static void Test_ButtonsTask(void)
{
    (void)BTN_Poll();
}


/* =========================================================
 * Mode Test
 *
 * D3 -> AUTO <-> MANUAL
 * ========================================================= */

static void Test_ModeTask(void)
{
    uint8 modePressed = 0U;

    if (BTN_WasPressed(
            BTN_MODE,
            &modePressed) != E_OK)
    {
        return;
    }


    if (modePressed != 0U)
    {
        if (g_manualMode == 0U)
        {
            g_manualMode = 1U;
        }
        else
        {
            g_manualMode = 0U;
        }

        Test_UpdateLEDs();
    }
}


/* =========================================================
 * Alarm Test
 *
 * D2 -> ALARM
 *
 * One press:
 *
 * B7    -> ON
 * Buzzer -> ON
 *
 * Alarm remains ON after releasing D2.
 * ========================================================= */

static void Test_AlarmTask(void)
{
    uint8 alarmPressed = 0U;

    if (BTN_WasPressed(
            BTN_RESET,
            &alarmPressed) != E_OK)
    {
        return;
    }


    if (alarmPressed != 0U)
    {
        /* LATCH ALARM */

        g_alarmActive = 1U;

        /* Alarm LED ON */

        (void)GPIO_SetPinValue(
            GPIO_PORTB,
            GPIO_PIN7,
            GPIO_LOW
        );

        /* Buzzer ON */

        (void)ACT_BuzzerOn();
    }
}


/* =========================================================
 * Timer0 ISR
 *
 * ~10 ms scheduler tick
 * ========================================================= */

ISR(TIMER0_COMP_vect)
{
    SCH_Tick();
}

int main(void)
{
    /* ---------------------------------------------------------
     * Hardware Initialization
     * --------------------------------------------------------- */

    (void)ACT_Init();

    (void)BTN_Init();


    /* ---------------------------------------------------------
     * PORT B LED Direction
     * --------------------------------------------------------- */

    (void)GPIO_SetPinDirection(
        GPIO_PORTB,
        GPIO_PIN5,
        GPIO_OUTPUT
    );

    (void)GPIO_SetPinDirection(
        GPIO_PORTB,
        GPIO_PIN6,
        GPIO_OUTPUT
    );

    (void)GPIO_SetPinDirection(
        GPIO_PORTB,
        GPIO_PIN7,
        GPIO_OUTPUT
    );


    /* ---------------------------------------------------------
     * Initial LED State
     *
     * Active-Low:
     * HIGH = OFF
     * --------------------------------------------------------- */

    (void)GPIO_SetPinValue(
        GPIO_PORTB,
        GPIO_PIN5,
        GPIO_HIGH
    );

    (void)GPIO_SetPinValue(
        GPIO_PORTB,
        GPIO_PIN6,
        GPIO_HIGH
    );

    (void)GPIO_SetPinValue(
        GPIO_PORTB,
        GPIO_PIN7,
        GPIO_HIGH
    );


    /* ---------------------------------------------------------
     * Buzzer OFF
     * --------------------------------------------------------- */

    (void)ACT_BuzzerOff();


    /* ---------------------------------------------------------
     * Scheduler
     * --------------------------------------------------------- */

    (void)SCH_Init();


    /* ---------------------------------------------------------
     * Buttons Polling
     *
     * 10 ms
     * --------------------------------------------------------- */

    (void)SCH_CreateTask(
        SCH_TASK_BUTTONS,
        Test_ButtonsTask,
        SCH_BUTTONS_PERIOD_MS
    );


    /* ---------------------------------------------------------
     * Mode Task
     *
     * 10 ms
     *
     * D3 -> AUTO / MANUAL
     * --------------------------------------------------------- */

    (void)SCH_CreateTask(
        SCH_TASK_FSM,
        Test_ModeTask,
        SCH_FSM_PERIOD_MS
    );


    /* ---------------------------------------------------------
     * Alarm Task
     *
     * 20 ms
     *
     * D2 -> Alarm
     * --------------------------------------------------------- */

    (void)SCH_CreateTask(
        SCH_TASK_CONSOLE,
        Test_AlarmTask,
        SCH_CONSOLE_PERIOD_MS
    );


    /* ---------------------------------------------------------
     * Timer0
     * --------------------------------------------------------- */

    (void)TIMER0_Init(TIMER0_CTC);

    (void)TIMER0_SetCompareValue(77U);

    (void)TIMER0_SetCompareInterrupt(1U);

    (void)TIMER0_Start(TIMER0_PRESC_1024);


    /* ---------------------------------------------------------
     * Enable Interrupts
     * --------------------------------------------------------- */

    sei();


    /* ---------------------------------------------------------
     * Super Loop
     * --------------------------------------------------------- */

    while (1)
    {
        SCH_Run();
    }


    return 0;
}