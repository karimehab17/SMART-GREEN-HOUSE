#ifndef GREENHOUSE_FSM_H
#define GREENHOUSE_FSM_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

typedef enum
{
    ST_INIT = 0,
    ST_AUTO,
    ST_MANUAL,
    ST_ALARM,
    ST_CONFIG
} GreenhouseStateType;

typedef enum
{
    FSM_OK = 0,
    FSM_ERROR
} FSM_StatusType;

FSM_StatusType GHSM_Init(Config_t *pConfig);

FSM_StatusType GHSM_Update(void);

GreenhouseStateType GHSM_GetState(void);

uint8 GHSM_IsAlarmLatched(void);

FSM_StatusType GHSM_RequestReset(void);

FSM_StatusType GHSM_RequestApply(void);

FSM_StatusType GHSM_RequestSetConfig(
    const Config_t *pConfig);

FSM_StatusType GHSM_RequestMode(GreenhouseStateType Copy_enState);

#endif /* GREENHOUSE_FSM_H */