#include "Actuators_Driver.h"

int main(void)
{
    ACT_Init();

    ACT_Set(ACTUATOR_BUZZER, ACT_STATE_ON);

    while (1)
    {
    }

    return 0;
}