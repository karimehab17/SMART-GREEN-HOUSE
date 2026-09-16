#include "scheduler.h"
#include <stddef.h>

/* =========================================================
 * Scheduler runtime data
 * ========================================================= */
static SCH_TaskType g_tasks[SCH_TASK_COUNT];

static volatile uint8 g_tickPending = 0U;

static uint16 g_overrunCount = 0U;


/* =========================================================
 * Convert milliseconds to scheduler ticks
 * ========================================================= */
static uint16 SCH_MsToTicks(uint16 periodMs)
{
    return (uint16)(periodMs / SCH_TICK_MS);
}


/* =========================================================
 * Initialize scheduler
 * ========================================================= */
STD_ReturnType SCH_Init(void)
{
    uint8 i;

    for (i = 0U; i < SCH_TASK_COUNT; i++)
    {
        g_tasks[i].callback = NULL;
        g_tasks[i].periodTicks = 0U;
        g_tasks[i].elapsedTicks = 0U;
        g_tasks[i].enabled = 0U;
    }

    g_tickPending = 0U;
    g_overrunCount = 0U;

    return E_OK;
}


/* =========================================================
 * Create / configure scheduler task
 * ========================================================= */
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

    if ((periodMs % SCH_TICK_MS) != 0U)
    {
        return E_NOK;
    }

    periodTicks = SCH_MsToTicks(periodMs);

    if (periodTicks == 0U)
    {
        return E_NOK;
    }

    g_tasks[taskId].callback = callback;
    g_tasks[taskId].periodTicks = periodTicks;
    g_tasks[taskId].elapsedTicks = 0U;
    g_tasks[taskId].enabled = 1U;

    return E_OK;
}


/* =========================================================
 * Enable task
 * ========================================================= */
STD_ReturnType SCH_EnableTask(uint8 taskId)
{
    if (taskId >= SCH_TASK_COUNT)
    {
        return E_NOK;
    }

    g_tasks[taskId].enabled = 1U;

    return E_OK;
}


/* =========================================================
 * Disable task
 * ========================================================= */
STD_ReturnType SCH_DisableTask(uint8 taskId)
{
    if (taskId >= SCH_TASK_COUNT)
    {
        return E_NOK;
    }

    g_tasks[taskId].enabled = 0U;

    return E_OK;
}


/* =========================================================
 * 10 ms scheduler tick
 *
 * Called from Timer0 ISR.
 * ========================================================= */
void SCH_Tick(void)
{
    if (g_tickPending != 0U)
    {
        if (g_overrunCount < 65535U)
        {
            g_overrunCount++;
        }
    }

    g_tickPending = 1U;
}


/* =========================================================
 * Execute one pending scheduler tick
 *
 * Cooperative / non-preemptive execution.
 * ========================================================= */
void SCH_Run(void)
{
    uint8 i;

    if (g_tickPending == 0U)
    {
        return;
    }

    g_tickPending = 0U;

    for (i = 0U; i < SCH_TASK_COUNT; i++)
    {
        if (g_tasks[i].enabled == 0U)
        {
            continue;
        }

        g_tasks[i].elapsedTicks++;

        if (g_tasks[i].elapsedTicks >= g_tasks[i].periodTicks)
        {
            g_tasks[i].elapsedTicks = 0U;

            if (g_tasks[i].callback != NULL)
            {
                g_tasks[i].callback();
            }
        }
    }
}


/* =========================================================
 * Get scheduler overrun count
 * ========================================================= */
uint16 SCH_GetOverrunCount(void)
{
    return g_overrunCount;
}