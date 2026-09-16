#include "greenhouse_fsm.h"

#include "Buttons_Driver.h"
#include "Sensors_Driver.h"
#include "Actuators_Driver.h"
#include "control.h"

#include <stddef.h>

/* =========================================================
 * Internal FSM data
 * ========================================================= */

static Config_t *g_pConfig = NULL;

static GreenhouseStateType g_currentState = ST_INIT;
static GreenhouseStateType g_previousState = ST_AUTO;

static uint8 g_alarmLatched = 0U;

static uint8 g_resetRequest = 0U;
static uint8 g_applyRequest = 0U;

static Config_t g_configSnapshot;
static Config_t g_pendingConfig;

static uint16 g_factoryResetTicks = 0U;
static uint8 g_alarmBeepTicks = 0U;


/* =========================================================
 * Check alarm conditions
 *
 * Alarm:
 * Temperature > tempAlarmC
 * Soil moisture < soilAlarmPct
 * ========================================================= */
static uint8 GHSM_IsAlarmConditionActive(void)
{
    uint16 rawTemp = 0U;
    uint16 rawSoil = 0U;

    uint8 tempC = 0U;
    uint8 soilPct = 0U;

    if (g_pConfig == NULL)
    {
        return 0U;
    }

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, NULL) != E_OK)
    {
        return 0U;
    }

    if (Sensors_ScaleTempC(rawTemp, &tempC) != E_OK)
    {
        return 0U;
    }

    if (Sensors_ScalePct(rawSoil, &soilPct) != E_OK)
    {
        return 0U;
    }

    if (tempC > g_pConfig->tempAlarmC)
    {
        return 1U;
    }

    if (soilPct < g_pConfig->soilAlarmPct)
    {
        return 1U;
    }

    return 0U;
}


/* =========================================================
 * Apply safe outputs during alarm
 * ========================================================= */
static void GHSM_ApplyAlarmOutputs(void)
{
    uint16 rawTemp = 0U;
    uint16 rawSoil = 0U;

    uint8 tempC = 0U;
    uint8 soilPct = 0U;

    (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_ON);

    if (Sensors_ReadRaw(&rawTemp, &rawSoil, NULL) != E_OK)
    {
        return;
    }

    if ((Sensors_ScaleTempC(rawTemp, &tempC) != E_OK) ||
        (Sensors_ScalePct(rawSoil, &soilPct) != E_OK))
    {
        return;
    }

    if (tempC > g_pConfig->tempAlarmC)
    {
        (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
    }

    if (soilPct < g_pConfig->soilAlarmPct)
    {
        (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON);
    }
}


/* =========================================================
 * Alarm buzzer
 *
 * 100 ms ON
 * 900 ms OFF
 *
 * Called every 10 ms.
 * ========================================================= */
static void GHSM_UpdateBuzzer(void)
{
    if (g_alarmBeepTicks < 10U)
    {
        (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_ON);
    }
    else
    {
        (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);
    }

    g_alarmBeepTicks++;

    if (g_alarmBeepTicks >= 100U)
    {
        g_alarmBeepTicks = 0U;
    }
}


/* =========================================================
 * Stop alarm outputs
 * ========================================================= */
static void GHSM_StopAlarm(void)
{
    (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_OFF);
    (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);

    g_alarmBeepTicks = 0U;
}


/* =========================================================
 * Factory reset
 *
 * Save + Reset held for 300 scheduler ticks = 3 seconds.
 * ========================================================= */
static void GHSM_HandleFactoryReset(void)
{
    ButtonStateType saveState = BTN_RELEASED;
    ButtonStateType resetState = BTN_RELEASED;

    if ((BTN_GetState(BTN_SAVE, &saveState) != E_OK) ||
        (BTN_GetState(BTN_RESET, &resetState) != E_OK))
    {
        g_factoryResetTicks = 0U;
        return;
    }

    if ((saveState == BTN_PRESSED) &&
        (resetState == BTN_PRESSED))
    {
        if (g_factoryResetTicks < SCH_FACTORY_RESET_TICKS)
        {
            g_factoryResetTicks++;
        }

        if (g_factoryResetTicks >= SCH_FACTORY_RESET_TICKS)
        {
            g_factoryResetTicks = 0U;

            g_pConfig->magic = CFG_MAGIC;
            g_pConfig->version = CFG_VERSION;

            g_pConfig->tempOnC = DEFAULT_TEMP_ON_C;
            g_pConfig->tempOffC = DEFAULT_TEMP_OFF_C;

            g_pConfig->soilOnPct = DEFAULT_SOIL_ON_PCT;
            g_pConfig->soilOffPct = DEFAULT_SOIL_OFF_PCT;

            g_pConfig->lightOnPct = DEFAULT_LIGHT_ON_PCT;
            g_pConfig->lightOffPct = DEFAULT_LIGHT_OFF_PCT;

            g_pConfig->tempAlarmC = DEFAULT_TEMP_ALARM_C;
            g_pConfig->soilAlarmPct = DEFAULT_SOIL_ALARM_PCT;

            g_pConfig->mode = DEFAULT_MODE;

            /*
             * Checksum is handled by the configuration/storage
             * module when Save/restore is integrated.
             */
            g_pConfig->checksum = 0U;

            (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);

            g_alarmLatched = 0U;
            g_resetRequest = 0U;
            g_applyRequest = 0U;

            g_currentState = ST_INIT;
        }
    }
    else
    {
        g_factoryResetTicks = 0U;
    }
}


/* =========================================================
 * Initialization
 * ========================================================= */
FSM_StatusType GHSM_Init(Config_t *pConfig)
{
    if (pConfig == NULL)
    {
        return FSM_ERROR;
    }

    g_pConfig = pConfig;

    g_currentState = ST_INIT;
    g_previousState = ST_AUTO;

    g_alarmLatched = 0U;

    g_resetRequest = 0U;
    g_applyRequest = 0U;

    g_factoryResetTicks = 0U;
    g_alarmBeepTicks = 0U;

    g_configSnapshot = *pConfig;
    g_pendingConfig = *pConfig;

    return FSM_OK;
}


/* =========================================================
 * Main FSM update
 *
 * Called every 10 ms.
 * ========================================================= */
FSM_StatusType GHSM_Update(void)
{
    uint8 modePressed = 0U;
    uint8 resetPressed = 0U;
    uint8 alarmActive = 0U;

    if (g_pConfig == NULL)
    {
        return FSM_ERROR;
    }

    /*
     * Factory reset has highest priority.
     */
    GHSM_HandleFactoryReset();

    /*
     * Read button press events independently.
     */
    if (BTN_WasPressed(BTN_MODE, &modePressed) != E_OK)
    {
        return FSM_ERROR;
    }

    if (BTN_WasPressed(BTN_RESET, &resetPressed) != E_OK)
    {
        return FSM_ERROR;
    }

    /*
     * Exactly one state switch in the FSM.
     */
    switch (g_currentState)
    {
        case ST_INIT:

            (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_OFF);
            (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);

            g_alarmLatched = 0U;

            g_currentState = ST_AUTO;

            break;


        case ST_AUTO:

            /*
             * Automatic control is owned by Control module.
             */
            (void)CTRL_UpdateThermal();
            (void)CTRL_UpdateIrrigation();
            (void)CTRL_UpdatePhoto();

            /*
             * Safety remains active.
             */
            alarmActive = GHSM_IsAlarmConditionActive();

            if (alarmActive != 0U)
            {
                g_previousState = ST_AUTO;
                g_alarmLatched = 1U;
                g_alarmBeepTicks = 0U;

                g_currentState = ST_ALARM;
            }
            else if (modePressed != 0U)
            {
                g_currentState = ST_MANUAL;
            }

            break;


        case ST_MANUAL:

            /*
             * Automatic loops are suspended in MANUAL.
             * Console controls the actuators.
             */

            alarmActive = GHSM_IsAlarmConditionActive();

            if (alarmActive != 0U)
            {
                g_previousState = ST_MANUAL;
                g_alarmLatched = 1U;
                g_alarmBeepTicks = 0U;

                g_currentState = ST_ALARM;
            }
            else if (modePressed != 0U)
            {
                g_currentState = ST_AUTO;
            }

            break;


        case ST_ALARM:

            /*
             * Alarm remains latched.
             */
            GHSM_ApplyAlarmOutputs();
            GHSM_UpdateBuzzer();

            if ((resetPressed != 0U) ||
                (g_resetRequest != 0U))
            {
                /*
                 * Reset is allowed only when the
                 * triggering condition is cleared.
                 */
                if (GHSM_IsAlarmConditionActive() == 0U)
                {
                    g_alarmLatched = 0U;

                    GHSM_StopAlarm();

                    g_resetRequest = 0U;

                    g_currentState = g_previousState;
                }
            }

            break;


        case ST_CONFIG:

            /*
             * Configuration editing is performed by
             * the Console module.
             */
            if (g_applyRequest != 0U)
            {
                *g_pConfig = g_pendingConfig;

                g_applyRequest = 0U;

                g_currentState = ST_AUTO;
            }

            break;


        default:

            g_currentState = ST_INIT;

            break;
    }

    return FSM_OK;
}


/* =========================================================
 * Get current state
 * ========================================================= */
GreenhouseStateType GHSM_GetState(void)
{
    return g_currentState;
}


/* =========================================================
 * Get alarm latch state
 * ========================================================= */
uint8 GHSM_IsAlarmLatched(void)
{
    return g_alarmLatched;
}


/* =========================================================
 * Request alarm reset
 *
 * Used by Console RESET command.
 * ========================================================= */
FSM_StatusType GHSM_RequestReset(void)
{
    g_resetRequest = 1U;

    return FSM_OK;
}


/* =========================================================
 * Request configuration apply
 *
 * Used by Console APPLY command / Save integration.
 * ========================================================= */
FSM_StatusType GHSM_RequestApply(void)
{
    g_applyRequest = 1U;

    return FSM_OK;
}


/* =========================================================
 * Enter configuration state
 *
 * Used by Console SET command.
 * ========================================================= */
FSM_StatusType GHSM_RequestSetConfig(const Config_t *pConfig)
{
    if ((g_pConfig == NULL) || (pConfig == NULL))
    {
        return FSM_ERROR;
    }

    g_configSnapshot = *g_pConfig;
    g_pendingConfig = *pConfig;

    g_currentState = ST_CONFIG;

    return FSM_OK;
}