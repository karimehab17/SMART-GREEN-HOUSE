#include "Actuators_Driver.h"
#include "../../MCAL/GPIO/gpio_interface.h"
#include "STD_TYPES.h"
#include "config.h"
#include <stddef.h>

#define ACTUATOR_FAN_PIN      GPIO_PIN0     /* PB0 */
#define ACTUATOR_PUMP_PIN     GPIO_PIN1     /* PB1 */
#define ACTUATOR_LAMP_PIN     GPIO_PIN2     /* PB2 */
#define ACTUATOR_ALARM_PIN    GPIO_PIN3     /* PB3 */

static uint8 ACT_GetPinForActuator(ActuatorType actuator, uint8 *pPort, uint8 *pPin)
{
    if ((pPort == NULL) || (pPin == NULL))
    {
        return 0U;
    }

    switch (actuator)
    {
        case ACTUATOR_FAN:   *pPort = GPIO_PORTB; *pPin = ACTUATOR_FAN_PIN;   return 1U;
        case ACTUATOR_PUMP:  *pPort = GPIO_PORTB; *pPin = ACTUATOR_PUMP_PIN;  return 1U;
        case ACTUATOR_LAMP:  *pPort = GPIO_PORTB; *pPin = ACTUATOR_LAMP_PIN;  return 1U;
        case ACTUATOR_ALARM:*pPort = GPIO_PORTB; *pPin = ACTUATOR_ALARM_PIN; return 1U;
        default:            return 0U;
    }
}

STD_ReturnType ACT_Init(void)
{
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_FAN_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_PUMP_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_LAMP_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }
    if (GPIO_SetPinDirection(GPIO_PORTB, ACTUATOR_ALARM_PIN, GPIO_OUTPUT) != E_OK)
    {
        return E_NOK;
    }

    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_FAN_PIN, GPIO_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_PUMP_PIN, GPIO_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_LAMP_PIN, GPIO_LOW);
    (void)GPIO_SetPinValue(GPIO_PORTB, ACTUATOR_ALARM_PIN, GPIO_LOW);

    return E_OK;
}

STD_ReturnType ACT_Set(ActuatorType actuator, ActuatorStateType state)
{
    uint8 port = 0U;
    uint8 pin = 0U;

    if (ACT_GetPinForActuator(actuator, &port, &pin) == 0U)
    {
        return E_NOK;
    }

    if ((state != ACT_STATE_OFF) && (state != ACT_STATE_ON))
    {
        return E_NOK;
    }

    return GPIO_SetPinValue(port, pin, (state == ACT_STATE_ON) ? GPIO_HIGH : GPIO_LOW);
}

STD_ReturnType ACT_SetAll(ActuatorStateType fanState, ActuatorStateType pumpState, ActuatorStateType lampState, ActuatorStateType alarmState)
{
    if ((ACT_Set(ACTUATOR_FAN, fanState) != E_OK) ||
        (ACT_Set(ACTUATOR_PUMP, pumpState) != E_OK) ||
        (ACT_Set(ACTUATOR_LAMP, lampState) != E_OK) ||
        (ACT_Set(ACTUATOR_ALARM, alarmState) != E_OK))
    {
        return E_NOK;
    }

    return E_OK;
}

STD_ReturnType ACT_Get(ActuatorType actuator, ActuatorStateType *pState)
{
    uint8 port = 0U;
    uint8 pin = 0U;
    uint8 level = GPIO_LOW;

    if (pState == NULL)
    {
        return E_NOK;
    }

    if (ACT_GetPinForActuator(actuator, &port, &pin) == 0U)
    {
        return E_NOK;
    }

    if (GPIO_GetPinValue(port, pin, &level) != E_OK)
    {
        return E_NOK;
    }

    *pState = (level == GPIO_HIGH) ? ACT_STATE_ON : ACT_STATE_OFF;

    return E_OK;

}
