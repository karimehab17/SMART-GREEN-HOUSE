#include "STD_TYPES.h"
#include "Sensors_Driver.h"
#include "Actuators_Driver.h"

int main(void)
{
    uint16 tempRaw = 0;
    uint16 soilRaw = 0;
    uint16 lightRaw = 0;

    uint8 temp = 0;
    uint8 soil = 0;
    uint8 light = 0;

    /* Initialize Sensors + Actuators */
    Sensors_Init();
    ACT_Init();

    while (1)
    {
        /* Read all sensors */
        Sensors_ReadRaw(&tempRaw, &soilRaw, &lightRaw);

        /* Convert ADC readings */
        Sensors_ScaleTempC(tempRaw, &temp);
        Sensors_ScalePct(soilRaw, &soil);
        Sensors_ScalePct(lightRaw, &light);

        /* FAN: Temperature > 25°C */
        if (temp > 25)
            ACT_Set(ACTUATOR_FAN, ACT_STATE_ON);
        else
            ACT_Set(ACTUATOR_FAN, ACT_STATE_OFF);

        /* PUMP: Soil < 50% */
        if (soil < 50)
            ACT_Set(ACTUATOR_PUMP, ACT_STATE_ON);
        else
            ACT_Set(ACTUATOR_PUMP, ACT_STATE_OFF);

        /* LAMP: Light < 50% */
        if (light < 50)
            ACT_Set(ACTUATOR_LAMP, ACT_STATE_ON);
        else
            ACT_Set(ACTUATOR_LAMP, ACT_STATE_OFF);

        /* ALARM: Temperature > 45°C */
        if (temp > 45)
            ACT_Set(ACTUATOR_ALARM, ACT_STATE_ON);
        else
            ACT_Set(ACTUATOR_ALARM, ACT_STATE_OFF);

        _delay_ms(200);
    }
}