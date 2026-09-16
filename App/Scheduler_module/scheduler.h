#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

/* ==================== Task Callback ==================== */

typedef void (*SCH_TaskCallbackType)(void);


/* ==================== Task Control Block ==================== */

typedef struct
{
    SCH_TaskCallbackType callback;
    uint16 periodTicks;
    uint16 elapsedTicks;
    uint8 enabled;
} SCH_TaskType;


/* ==================== Scheduler API ==================== */

STD_ReturnType SCH_Init(void);

STD_ReturnType SCH_CreateTask(
    uint8 Copy_u8TaskId,
    SCH_TaskCallbackType Copy_pfCallback,
    uint16 Copy_u16PeriodMs);

STD_ReturnType SCH_EnableTask(
    uint8 Copy_u8TaskId);

STD_ReturnType SCH_DisableTask(
    uint8 Copy_u8TaskId);

void SCH_Tick(void);

void SCH_Run(void);

uint16 SCH_GetOverrunCount(void);

#endif /* SCHEDULER_H */