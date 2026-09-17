#include "greenhouse_fsm.h"

#include "../../HAl/Buttons/Buttons_Driver.h"
#include "../../HAl/Sensors/Sensors_Driver.h"
#include "../../HAl/Actuators/Actuators_Driver.h"
#include <avr/eeprom.h>

#include <stddef.h>


static Config_t *g_pConfig = NULL;

static GreenhouseStateType g_currentState = ST_INIT;
static GreenhouseStateType g_previousState = ST_AUTO;

static uint8 g_alarmLatched = 0U;
static uint8 g_resetRequest = 0U;
static uint8 g_applyRequest = 0U;
static uint8 g_modeRequest = 0U;

static GreenhouseStateType g_requestedMode = ST_AUTO;

static Config_t g_pendingConfig;

static uint16 g_factoryResetTicks = 0U;
static uint8 g_alarmBeepTicks = 0U;


/* =========================================================
 * Alarm Condition
 * ========================================================= */

static uint8 GHSM_IsAlarmConditionActive(void)
{
    uint8 Local_u8TempC;
    uint8 Local_u8SoilPct;

    if (Sensors_IsReady() == 0U)
    {
        return 0U;
    }

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

/* =========================================================
 * Actuator Helpers
 * ========================================================= */

static void GHSM_SetAllActuatorsOff(void)
{
    (void)ACT_Set(
        ACTUATOR_FAN,
        ACT_STATE_OFF);

    (void)ACT_Set(
        ACTUATOR_PUMP,
        ACT_STATE_OFF);

    (void)ACT_Set(
        ACTUATOR_LAMP,
        ACT_STATE_OFF);

    (void)ACT_Set(
        ACTUATOR_ALARM,
        ACT_STATE_OFF);

    (void)ACT_Set(
        ACTUATOR_BUZZER,
        ACT_STATE_OFF);
}


static void GHSM_UpdateAlarmFan(uint8 Copy_u8TempC)
{
    if (Copy_u8TempC > g_pConfig->tempAlarmC)
    {
        (void)ACT_Set(
            ACTUATOR_FAN,
            ACT_STATE_ON);
    }
    else
    {
        (void)ACT_Set(
            ACTUATOR_FAN,
            ACT_STATE_OFF);
    }
}


static void GHSM_UpdateAlarmPump(uint8 Copy_u8SoilPct)
{
    if (Copy_u8SoilPct < g_pConfig->soilAlarmPct)
    {
        (void)ACT_Set(
            ACTUATOR_PUMP,
            ACT_STATE_ON);
    }
    else
    {
        (void)ACT_Set(
            ACTUATOR_PUMP,
            ACT_STATE_OFF);
    }
}


static void GHSM_ApplyAlarmOutputs(void)
{
    uint8 Local_u8TempC;
    uint8 Local_u8SoilPct;

    (void)ACT_Set(
        ACTUATOR_ALARM,
        ACT_STATE_ON);

    if (Sensors_GetTemperature(&Local_u8TempC) == E_OK)
    {
        GHSM_UpdateAlarmFan(Local_u8TempC);
    }

    if (Sensors_GetSoil(&Local_u8SoilPct) == E_OK)
    {
        GHSM_UpdateAlarmPump(Local_u8SoilPct);
    }
}


/* =========================================================
 * Alarm Buzzer
 * ========================================================= */

static void GHSM_UpdateBuzzer(void)
{
    if (g_alarmBeepTicks < SCH_ALARM_ON_TICKS)
    {
        (void)ACT_Set(
            ACTUATOR_BUZZER,
            ACT_STATE_ON);
    }
    else
    {
        (void)ACT_Set(
            ACTUATOR_BUZZER,
            ACT_STATE_OFF);
    }

    g_alarmBeepTicks++;

    if (g_alarmBeepTicks >= SCH_ALARM_PERIOD_TICKS)
    {
        g_alarmBeepTicks = 0U;
    }
}


static void GHSM_StopAlarm(void)
{
    (void)ACT_Set(
        ACTUATOR_ALARM,
        ACT_STATE_OFF);

    (void)ACT_Set(
        ACTUATOR_BUZZER,
        ACT_STATE_OFF);

    g_alarmBeepTicks = 0U;
}


/* =========================================================
 * Factory Reset
 * ========================================================= */

static void GHSM_LoadDefaultConfig(void)
{
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
}


static void GHSM_CompleteFactoryReset(void)
{
    GHSM_LoadDefaultConfig();

    g_pendingConfig = *g_pConfig;

    g_alarmLatched = 0U;
    g_resetRequest = 0U;
    g_applyRequest = 0U;
    g_modeRequest = 0U;

    GHSM_SetAllActuatorsOff();

    g_currentState = ST_INIT;
}


static void GHSM_HandleFactoryReset(void)
{
    ButtonStateType Local_SaveState;
    ButtonStateType Local_ResetState;

    Local_SaveState = BTN_RELEASED;
    Local_ResetState = BTN_RELEASED;

    if ((BTN_GetState(
             BTN_SAVE,
             &Local_SaveState) != E_OK) ||
        (BTN_GetState(
             BTN_RESET,
             &Local_ResetState) != E_OK))
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

            GHSM_CompleteFactoryReset();
        }
    }
    else
    {
        g_factoryResetTicks = 0U;
    }
}


/* =========================================================
 * Alarm Logic
 * ========================================================= */

static void GHSM_EnterAlarm(void)
{
    if (g_alarmLatched == 0U)
    {
        g_alarmLatched = 1U;
        g_alarmBeepTicks = 0U;
    }
}


static void GHSM_UpdateAlarmLogic(
    uint8 Copy_u8ResetPressed)
{
    uint8 Local_u8AlarmActive;

    Local_u8AlarmActive =
        GHSM_IsAlarmConditionActive();

    if (Local_u8AlarmActive != 0U)
    {
        GHSM_EnterAlarm();
    }

    if (g_alarmLatched != 0U)
    {
        GHSM_ApplyAlarmOutputs();
        GHSM_UpdateBuzzer();

        if (((Copy_u8ResetPressed != 0U) ||
             (g_resetRequest != 0U)) &&
            (Local_u8AlarmActive == 0U))
        {
            g_alarmLatched = 0U;
            g_resetRequest = 0U;

            GHSM_StopAlarm();
        }
    }
}


/* =========================================================
 * AUTO State
 * ========================================================= */

static void GHSM_UpdateAuto(
    uint8 Copy_u8ModePressed,
    uint8 Copy_u8ResetPressed)
{
    GHSM_UpdateAlarmLogic(
        Copy_u8ResetPressed);

    if (Copy_u8ModePressed != 0U)
    {
        g_currentState = ST_MANUAL;
    }
}


/* =========================================================
 * MANUAL State
 * ========================================================= */

static void GHSM_UpdateManual(
    uint8 Copy_u8ModePressed,
    uint8 Copy_u8ResetPressed)
{
    GHSM_UpdateAlarmLogic(
        Copy_u8ResetPressed);

    if (Copy_u8ModePressed != 0U)
    {
        g_currentState = ST_AUTO;
    }
}


/* =========================================================
 * Legacy Alarm State
 * ========================================================= */

static void GHSM_UpdateAlarm(
    uint8 Copy_u8ResetPressed)
{
    GHSM_UpdateAlarmLogic(
        Copy_u8ResetPressed);
}


/* =========================================================
 * Configuration State
 * ========================================================= */

static void GHSM_UpdateConfig(void)
{
    if (g_applyRequest != 0U)
    {
        *g_pConfig = g_pendingConfig;

        g_applyRequest = 0U;

        g_currentState = ST_AUTO;
    }
}


/* =========================================================
 * Requested Mode
 * ========================================================= */

static void GHSM_ProcessModeRequest(void)
{
    if (g_modeRequest == 0U)
    {
        return;
    }

    g_modeRequest = 0U;

    if (g_requestedMode == ST_MANUAL)
    {
        g_currentState = ST_MANUAL;
    }
    else
    {
        g_currentState = ST_AUTO;
    }
}


/* =========================================================
 * Configuration Checksum
 * ========================================================= */

static uint8 GHSM_CalculateChecksum(
    const Config_t *pConfig)
{
    const uint8 *pData;
    uint8 Local_u8Checksum = 0U;
    uint8 Local_u8Index;

    if (pConfig == NULL)
    {
        return 0U;
    }

    pData = (const uint8 *)pConfig;

    for (Local_u8Index = 0U;
         Local_u8Index < (uint8)(sizeof(Config_t) - 1U);
         Local_u8Index++)
    {
        Local_u8Checksum =
            (uint8)(
                Local_u8Checksum +
                pData[Local_u8Index]);

        }

    return (uint8)(0U - Local_u8Checksum);
}


/* =========================================================
 * EEPROM Save
 * ========================================================= */

FSM_StatusType GHSM_SaveConfig(void)
{
    Config_t Local_Config;

    if (g_pConfig == NULL)
    {
        return FSM_ERROR;
    }

    Local_Config = *g_pConfig;

    Local_Config.magic = CFG_MAGIC;
    Local_Config.version = CFG_VERSION;

    Local_Config.checksum =
        GHSM_CalculateChecksum(
            &Local_Config);

    eeprom_update_block(
        &Local_Config,
        (void *)(uintptr_t)
            EEPROM_CONFIG_ADDRESS,
        sizeof(Config_t));

    *g_pConfig = Local_Config;

    return FSM_OK;
}


/* =========================================================
 * EEPROM Load
 * ========================================================= */

static uint8 GHSM_IsConfigValid(
    const Config_t *pConfig)
{
    if ((pConfig->magic != CFG_MAGIC) ||
        (pConfig->version != CFG_VERSION))
    {
        return 0U;
    }

    return (uint8)(
        pConfig->checksum ==
        GHSM_CalculateChecksum(pConfig));
}


static void GHSM_LoadConfigFromEEPROM(void)
{
    Config_t Local_Config;

    eeprom_read_block(
        &Local_Config,
        (const void *)(uintptr_t)
            EEPROM_CONFIG_ADDRESS,
        sizeof(Config_t));

    if (GHSM_IsConfigValid(
            &Local_Config) != 0U)
    {
        *g_pConfig = Local_Config;
    }
    else
    {
        GHSM_LoadDefaultConfig();
    }
}


/* =========================================================
 * Initialization
 * ========================================================= */

FSM_StatusType GHSM_Init(
    Config_t *pConfig)
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
    g_modeRequest = 0U;

    g_requestedMode = ST_AUTO;

    g_factoryResetTicks = 0U;
    g_alarmBeepTicks = 0U;

    GHSM_LoadConfigFromEEPROM();

    g_pendingConfig = *g_pConfig;

    return FSM_OK;
}


/* =========================================================
 * Main FSM Task
 * ========================================================= */

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

    GHSM_ProcessModeRequest();

    switch (g_currentState)
    {
        case ST_INIT:

            GHSM_SetAllActuatorsOff();

            g_alarmLatched = 0U;

            g_currentState = ST_AUTO;

            break;


        case ST_AUTO:

            GHSM_UpdateAuto(
                Local_u8ModePressed,
                Local_u8ResetPressed);

            break;


        case ST_MANUAL:

            GHSM_UpdateManual(
                Local_u8ModePressed,
                Local_u8ResetPressed);

            break;


        case ST_ALARM:

            GHSM_UpdateAlarm(
                Local_u8ResetPressed);

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


/* =========================================================
 * State Information
 * ========================================================= */

GreenhouseStateType GHSM_GetState(void)
{
    return g_currentState;
}


uint8 GHSM_IsAlarmLatched(void)
{
    return g_alarmLatched;
}


/* =========================================================
 * Console Requests
 * ========================================================= */

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


FSM_StatusType GHSM_RequestMode(
    GreenhouseStateType Copy_enState)
{
    if ((Copy_enState != ST_AUTO) &&
        (Copy_enState != ST_MANUAL))
    {
        return FSM_ERROR;
    }

    g_requestedMode = Copy_enState;
    g_modeRequest = 1U;

    return FSM_OK;
}