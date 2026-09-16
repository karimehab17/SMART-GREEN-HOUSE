#include "scheduler.h"
#include <stddef.h>

static SCH_TaskType g_tasks[SCH_TASK_COUNT];

static volatile uint8 g_tickPending = 0u;

static uint16 g_overrunCount = 0u;

static uint16 SCH_MsToTicks(uint16 periodMs)
{
    return (uint16)(periodMs / SCH_TICK_MS);
}

STD_ReturnType SCH_Init(void)
{
    uint8 i;

    for (i = 0u; i < SCH_TASK_COUNT; i++)
    {
        g_tasks[i].callback = NULL;
        g_tasks[i].periodTicks = 0u;
        g_tasks[i].elapsedTicks = 0u;
        g_tasks[i].enabled = 0u;
    }

    g_tickPending = 0u;
    g_overrunCount = 0u;

    return E_OK;
}

STD_ReturnType SCH_CreateTask(uint8 taskId,
                              SCH_TaskCallbackType callback,
                              uint16 periodMs)
{
    uint16 periodTicks;

    if ((taskId >= SCH_TASK_COUNT) ||
        (callback == NULL) ||
        (periodMs < SCH_TICK_MS))
    {
        return E_NOK;
    }

    if ((periodMs % SCH_TICK_MS) != 0u)
    {
        return E_NOK;
    }

    periodTicks = SCH_MsToTicks(periodMs);

    if (periodTicks == 0u)
    {
        return E_NOK;
    }

    g_tasks[taskId].callback = callback;
    g_tasks[taskId].periodTicks = periodTicks;
    g_tasks[taskId].elapsedTicks = 0u;
    g_tasks[taskId].enabled = 1u;

    return E_OK;
}

STD_ReturnType SCH_EnableTask(uint8 taskId)
{
    if (taskId >= SCH_TASK_COUNT)
    {
        return E_NOK;
    }

    g_tasks[taskId].enabled = 1u;

    return E_OK;
}

STD_ReturnType SCH_DisableTask(uint8 taskId)
{
    if (taskId >= SCH_TASK_COUNT)
    {
        return E_NOK;
    }

    g_tasks[taskId].enabled = 0u;

    return E_OK;
}

void SCH_Tick(void)
{
    if (g_tickPending == 1u)
    {
        g_overrunCount++;
    }

    g_tickPending = 1u;
}

void SCH_Run(void)
{
    uint8 i;

    if (g_tickPending == 0u)
    {
        return;
    }

    g_tickPending = 0u;

    for (i = 0u; i < SCH_TASK_COUNT; i++)
    {
        if (g_tasks[i].enabled == 1u)
        {
            g_tasks[i].elapsedTicks++;

            if (g_tasks[i].elapsedTicks >=
                g_tasks[i].periodTicks)
            {
                g_tasks[i].elapsedTicks = 0u;

                if (g_tasks[i].callback != NULL)
                {
                    g_tasks[i].callback();
                }
            }
        }
    }
}

uint16 SCH_GetOverrunCount(void)
{
    return g_overrunCount;
}