#include "scheduler.h"

#include <stddef.h>


/* ==================== Runtime Data ==================== */

static SCH_TaskType g_tasks[SCH_TASK_COUNT];

static volatile uint8 g_tickPending = 0U;

static uint16 g_overrunCount = 0U;


/* ==================== Private Functions ==================== */

static uint16 SCH_MsToTicks(uint16 Copy_u16TimeMs)
{
    return (uint16)(Copy_u16TimeMs / SCH_TICK_MS);
}


/* ==================== Initialization ==================== */

STD_ReturnType SCH_Init(void)
{
    uint8 Local_u8Index;

    for (Local_u8Index = 0U;
         Local_u8Index < SCH_TASK_COUNT;
         Local_u8Index++)
    {
        g_tasks[Local_u8Index].callback = NULL;
        g_tasks[Local_u8Index].periodTicks = 0U;
        g_tasks[Local_u8Index].elapsedTicks = 0U;
        g_tasks[Local_u8Index].enabled = 0U;
    }

    g_tickPending = 0U;
    g_overrunCount = 0U;

    return E_OK;
}


/* ==================== Create Task ==================== */

STD_ReturnType SCH_CreateTask(
    uint8 Copy_u8TaskId,
    SCH_TaskCallbackType Copy_pfCallback,
    uint16 Copy_u16PeriodMs)
{
    uint16 Local_u16PeriodTicks;

    if ((Copy_u8TaskId >= SCH_TASK_COUNT) ||
        (Copy_pfCallback == NULL))
    {
        return E_NOK;
    }

    if ((Copy_u16PeriodMs < SCH_TICK_MS) ||
        ((Copy_u16PeriodMs % SCH_TICK_MS) != 0U))
    {
        return E_NOK;
    }

    Local_u16PeriodTicks =
        SCH_MsToTicks(Copy_u16PeriodMs);

    if (Local_u16PeriodTicks == 0U)
    {
        return E_NOK;
    }

    g_tasks[Copy_u8TaskId].callback =
        Copy_pfCallback;

    g_tasks[Copy_u8TaskId].periodTicks =
        Local_u16PeriodTicks;

    g_tasks[Copy_u8TaskId].elapsedTicks = 0U;

    g_tasks[Copy_u8TaskId].enabled = 1U;

    return E_OK;
}


/* ==================== Enable Task ==================== */

STD_ReturnType SCH_EnableTask(uint8 Copy_u8TaskId)
{
    if (Copy_u8TaskId >= SCH_TASK_COUNT)
    {
        return E_NOK;
    }

    g_tasks[Copy_u8TaskId].enabled = 1U;

    return E_OK;
}


/* ==================== Disable Task ==================== */

STD_ReturnType SCH_DisableTask(uint8 Copy_u8TaskId)
{
    if (Copy_u8TaskId >= SCH_TASK_COUNT)
    {
        return E_NOK;
    }

    g_tasks[Copy_u8TaskId].enabled = 0U;

    return E_OK;
}


/* ==================== 10 ms System Tick ==================== */

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


/* ==================== Cooperative Dispatcher ==================== */

void SCH_Run(void)
{
    uint8 Local_u8Index;

    if (g_tickPending == 0U)
    {
        return;
    }

    g_tickPending = 0U;

    for (Local_u8Index = 0U;
         Local_u8Index < SCH_TASK_COUNT;
         Local_u8Index++)
    {
        if (g_tasks[Local_u8Index].enabled == 0U)
        {
            continue;
        }

        g_tasks[Local_u8Index].elapsedTicks++;

        if (g_tasks[Local_u8Index].elapsedTicks >=
            g_tasks[Local_u8Index].periodTicks)
        {
            g_tasks[Local_u8Index].elapsedTicks = 0U;

            if (g_tasks[Local_u8Index].callback != NULL)
            {
                g_tasks[Local_u8Index].callback();
            }
        }
    }
}


/* ==================== Overrun Counter ==================== */

uint16 SCH_GetOverrunCount(void)
{
    return g_overrunCount;
}