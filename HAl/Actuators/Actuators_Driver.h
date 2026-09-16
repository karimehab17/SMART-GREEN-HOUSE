#ifndef ACTUATORS_DRIVER_H
#define ACTUATORS_DRIVER_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

typedef enum
{
    ACTUATOR_FAN = 0U,
    ACTUATOR_PUMP,
    ACTUATOR_LAMP,
    ACTUATOR_ALARM,
    ACTUATOR_BUZZER,
    ACTUATOR_COUNT
} ActuatorType;

typedef enum
{
    ACT_STATE_OFF = 0U,
    ACT_STATE_ON = 1U
} ActuatorStateType;

STD_ReturnType ACT_Init(void);

STD_ReturnType ACT_Set(ActuatorType actuator, ActuatorStateType state);

STD_ReturnType ACT_SetAll(ActuatorStateType fanState, ActuatorStateType pumpState,ActuatorStateType lampState, ActuatorStateType alarmState);

STD_ReturnType ACT_Get(ActuatorType actuator, ActuatorStateType *pState);

STD_ReturnType ACT_BuzzerOn(void);

STD_ReturnType ACT_BuzzerOff(void);

STD_ReturnType ACT_BuzzerToggle(void);

#endif /* ACTUATORS_DRIVER_H */