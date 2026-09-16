#include "greenhouse_fsm.h"

#include "../../HAl/Buttons/Buttons_Driver.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include "config.h"

#include <stddef.h>

static Config_t *g_pConfig = NULL;

static GreenhouseStateType g_currentState = ST_INIT;
static GreenhouseStateType g_previousState = ST_AUTO;

static uint8 g_alarmLatched = 0U;
static uint8 g_resetRequest = 0U;
static uint8 g_applyRequest = 0U;

static Config_t g_pendingConfig;

static uint16 g_factoryResetTicks = 0U;
static uint8 g_alarmBeepTicks = 0U;

static uint8 GHSM_IsAlarmConditionActive(void)
{
    uint8 Local_u8TempC;
    uint8 Local_u8SoilPct;

    if (g_pConfig == NULL)
    {
        return 0U;
    }

    if (Sensors_GetTemperature(&Local_u8TempC) != E_OK)
    {
        return 0U;
    }

    if (Sensors_GetSoil(&Local_u8SoilPct) != E_OK)
    {
        return 0U;
    }

    if (Local_u8TempC > g_pConfig->tempAlarmC)
    {
        return 1U;
    }

    if (Local_u8SoilPct < g_pConfig->soilAlarmPct)
    {
        return 1U;
    }

    return 0U;
}

static void GHSM_SetAllActuatorsOff(void)
{
    (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);
    (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);
    (void)ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);
    (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_OFF);
    (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);
}

static void GHSM_ApplyAlarmOutputs(void)
{
    uint8 Local_u8TempC;
    uint8 Local_u8SoilPct;

    (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_ON);

    if (Sensors_GetTemperature(&Local_u8TempC) == E_OK)
    {
        if (Local_u8TempC > g_pConfig->tempAlarmC)
        {
            (void)ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
        }
    }

    if (Sensors_GetSoil(&Local_u8SoilPct) == E_OK)
    {
        if (Local_u8SoilPct < g_pConfig->soilAlarmPct)
        {
            (void)ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON);
        }
    }
}

static void GHSM_UpdateBuzzer(void)
{
    if (g_alarmBeepTicks < SCH_ALARM_ON_TICKS)
    {
        (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_ON);
    }
    else
    {
        (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);
    }

    g_alarmBeepTicks++;

    if (g_alarmBeepTicks >= SCH_ALARM_PERIOD_TICKS)
    {
        g_alarmBeepTicks = 0U;
    }
}

static void GHSM_StopAlarm(void)
{
    (void)ACT_Set(ACTUATOR_ALARM, ACT_STATE_OFF);
    (void)ACT_Set(ACTUATOR_BUZZER, ACT_STATE_OFF);

    g_alarmBeepTicks = 0U;
}

static void GHSM_HandleFactoryReset(void)
{
    ButtonStateType Local_SaveState;
    ButtonStateType Local_ResetState;

    Local_SaveState = BTN_RELEASED;
    Local_ResetState = BTN_RELEASED;

    if ((BTN_GetState(BTN_SAVE, &Local_SaveState) != E_OK) ||
        (BTN_GetState(BTN_RESET, &Local_ResetState) != E_OK))
    {
        g_factoryResetTicks = 0U;
        return;
    }

    if ((Local_SaveState == BTN_PRESSED) &&
        (Local_ResetState == BTN_PRESSED))
    {
        if (g_factoryResetTicks < FACTORY_RESET_TICKS)
        {
            g_factoryResetTicks++;
        }

        if (g_factoryResetTicks >= FACTORY_RESET_TICKS)
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

            g_alarmLatched = 0U;
            g_resetRequest = 0U;
            g_applyRequest = 0U;

            GHSM_SetAllActuatorsOff();

            g_currentState = ST_INIT;
        }
    }
    else
    {
        g_factoryResetTicks = 0U;
    }
}

static void GHSM_EnterAlarm(void)
{
    g_previousState = g_currentState;
    g_alarmLatched = 1U;
    g_alarmBeepTicks = 0U;
    g_currentState = ST_ALARM;
}

static void GHSM_UpdateAuto(uint8 modePressed)
{
    if (GHSM_IsAlarmConditionActive() != 0U)
    {
        GHSM_EnterAlarm();
    }
    else if (modePressed != 0U)
    {
        g_currentState = ST_MANUAL;
    }
}

static void GHSM_UpdateManual(uint8 modePressed)
{
    if (GHSM_IsAlarmConditionActive() != 0U)
    {
        GHSM_EnterAlarm();
    }
    else if (modePressed != 0U)
    {
        g_currentState = ST_AUTO;
    }
}

static void GHSM_UpdateAlarm(uint8 resetPressed)
{
    GHSM_ApplyAlarmOutputs();
    GHSM_UpdateBuzzer();

    if ((resetPressed != 0U) ||
        (g_resetRequest != 0U))
    {
        if (GHSM_IsAlarmConditionActive() == 0U)
        {
            g_alarmLatched = 0U;
            g_resetRequest = 0U;

            GHSM_StopAlarm();

            g_currentState = g_previousState;
        }
    }
}

static void GHSM_UpdateConfig(void)
{
    if (g_applyRequest != 0U)
    {
        *g_pConfig = g_pendingConfig;

        g_applyRequest = 0U;
        g_currentState = ST_AUTO;
    }
}

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

    g_pendingConfig = *pConfig;

    return FSM_OK;
}

FSM_StatusType GHSM_Update(void)
{
    uint8 Local_u8ModePressed;
    uint8 Local_u8ResetPressed;

    if (g_pConfig == NULL)
    {
        return FSM_ERROR;
    }

    GHSM_HandleFactoryReset();

    if (BTN_WasPressed(
            BTN_MODE,
            &Local_u8ModePressed) != E_OK)
    {
        return FSM_ERROR;
    }

    if (BTN_WasPressed(
            BTN_RESET,
            &Local_u8ResetPressed) != E_OK)
    {
        return FSM_ERROR;
    }

    switch (g_currentState)
    {
        case ST_INIT:
            GHSM_SetAllActuatorsOff();
            g_alarmLatched = 0U;
            g_currentState = ST_AUTO;
            break;

        case ST_AUTO:
            GHSM_UpdateAuto(Local_u8ModePressed);
            break;

        case ST_MANUAL:
            GHSM_UpdateManual(Local_u8ModePressed);
            break;

        case ST_ALARM:
            GHSM_UpdateAlarm(Local_u8ResetPressed);
            break;

        case ST_CONFIG:
            GHSM_UpdateConfig();
            break;

        default:
            g_currentState = ST_INIT;
            break;
    }

    return FSM_OK;
}

GreenhouseStateType GHSM_GetState(void)
{
    return g_currentState;
}

uint8 GHSM_IsAlarmLatched(void)
{
    return g_alarmLatched;
}

FSM_StatusType GHSM_RequestReset(void)
{
    g_resetRequest = 1U;

    return FSM_OK;
}

FSM_StatusType GHSM_RequestApply(void)
{
    g_applyRequest = 1U;

    return FSM_OK;
}

FSM_StatusType GHSM_RequestSetConfig(
    const Config_t *pConfig)
{
    if ((g_pConfig == NULL) ||
        (pConfig == NULL))
    {
        return FSM_ERROR;
    }

    g_pendingConfig = *pConfig;
    g_currentState = ST_CONFIG;

    return FSM_OK;
}