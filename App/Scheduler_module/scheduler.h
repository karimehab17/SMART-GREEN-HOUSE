#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "STD_TYPES.h"
#include "Math.h"
#include "config.h"

typedef void (*SCH_TaskCallbackType)(void);

typedef struct
{
    SCH_TaskCallbackType callback;
    uint16 periodTicks;
    uint16 elapsedTicks;
    uint8 enabled;
} SCH_TaskType;

STD_ReturnType SCH_Init(void);

STD_ReturnType SCH_CreateTask(uint8 taskId,
                              SCH_TaskCallbackType callback,
                              uint16 periodMs);

STD_ReturnType SCH_EnableTask(uint8 taskId);
STD_ReturnType SCH_DisableTask(uint8 taskId);

void SCH_Tick(void);
void SCH_Run(void);

uint16 SCH_GetOverrunCount(void);

#endif