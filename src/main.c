#include "STD_TYPES.h"
#include "config.h"
#include "control.h"

int main(void)
{
    Config_t config =
    {
        .magic = CFG_MAGIC,
        .version = CFG_VERSION,

        .tempOnC = 35u,
        .tempOffC = 32u,

        .soilOnPct = 40u,
        .soilOffPct = 60u,

        .lightOnPct = 25u,
        .lightOffPct = 40u,

        .tempAlarmC = 45u,
        .soilAlarmPct = 15u,

        .mode = 0u,
        .checksum = 0u
    };

    if (CTRL_Init(&config) != CONTROL_OK)
    {
        while (1)
        {
            /* Initialization error */
        }
    }

    while (1)
    {
        /*
         * Temporary integration point.
         * Scheduler will call the control tasks later.
         */
    }

    return 0;
}