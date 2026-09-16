#ifndef GREENHOUSE_FSM_H
#define GREENHOUSE_FSM_H

#include "STD_TYPES.h"
#include "config.h"

/* =========================================================
 * Greenhouse FSM States
 * ========================================================= */
typedef enum
{
    ST_INIT = 0,
    ST_AUTO,
    ST_MANUAL,
    ST_ALARM,
    ST_CONFIG
} GreenhouseStateType;

/* =========================================================
 * FSM Status
 * ========================================================= */
typedef enum
{
    FSM_OK = 0,
    FSM_ERROR
} FSM_StatusType;

/* =========================================================
 * Initialization
 * ========================================================= */
FSM_StatusType GHSM_Init(Config_t *pConfig);

/* =========================================================
 * Main FSM task
 *
 * Called by Scheduler every 10 ms.
 * ========================================================= */
FSM_StatusType GHSM_Update(void);

/* =========================================================
 * State information
 * ========================================================= */
GreenhouseStateType GHSM_GetState(void);

uint8 GHSM_IsAlarmLatched(void);

/* =========================================================
 * Console events
 * ========================================================= */
FSM_StatusType GHSM_RequestReset(void);

FSM_StatusType GHSM_RequestApply(void);

FSM_StatusType GHSM_RequestSetConfig(const Config_t *pConfig);

#endif /* GREENHOUSE_FSM_H */