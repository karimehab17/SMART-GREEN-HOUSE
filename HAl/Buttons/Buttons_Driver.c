#include "Buttons_Driver.h"
#include "../../MCAL/GPIO/gpio_interface.h"
#include <stddef.h>

#define BTN_DEBOUNCE_SAMPLES    2U

typedef struct
{
    ButtonStateType currentState;
    ButtonStateType lastRawState;
    uint8 debounceCount;
    uint8 edgeFlag;
} ButtonDebounceType;

static ButtonDebounceType g_buttonState[BTN_COUNT];

/*
 * Button pin mapping according to project README:
 *
 * BTN_RESET -> PD2 / INT0
 * BTN_MODE  -> PD3 / INT1
 * BTN_SAVE  -> PD4
 */
static uint8 BTN_GetPinForButton(ButtonType button,
                                 uint8 *pPort,
                                 uint8 *pPin)
{
    if ((pPort == NULL) || (pPin == NULL))
    {
        return 0U;
    }

    switch (button)
    {
        case BTN_RESET:
            *pPort = GPIO_PORTD;
            *pPin = GPIO_PIN2;
            return 1U;

        case BTN_MODE:
            *pPort = GPIO_PORTD;
            *pPin = GPIO_PIN3;
            return 1U;

        case BTN_SAVE:
            *pPort = GPIO_PORTD;
            *pPin = GPIO_PIN4;
            return 1U;

        default:
            return 0U;
    }
}

STD_ReturnType BTN_Init(void)
{
    uint8 index;

    for (index = 0U; index < BTN_COUNT; index++)
    {
        g_buttonState[index].currentState = BTN_RELEASED;
        g_buttonState[index].lastRawState = BTN_RELEASED;
        g_buttonState[index].debounceCount = 0U;
        g_buttonState[index].edgeFlag = 0U;
    }

    /*
     * All buttons use the ATmega32 internal pull-up.
     * Buttons are therefore active LOW.
     */

    if (GPIO_SetPinDirection(GPIO_PORTD,
                             GPIO_PIN2,
                             GPIO_INPUT_PULLUP) != E_OK)
    {
        return E_NOK;
    }

    if (GPIO_SetPinDirection(GPIO_PORTD,
                             GPIO_PIN3,
                             GPIO_INPUT_PULLUP) != E_OK)
    {
        return E_NOK;
    }

    if (GPIO_SetPinDirection(GPIO_PORTD,
                             GPIO_PIN4,
                             GPIO_INPUT_PULLUP) != E_OK)
    {
        return E_NOK;
    }

    return E_OK;
}

STD_ReturnType BTN_Poll(void)
{
    uint8 index;

    for (index = 0U; index < BTN_COUNT; index++)
    {
        uint8 port = 0U;
        uint8 pin = 0U;
        uint8 rawLevel = GPIO_LOW;
        ButtonStateType rawState;
        ButtonDebounceType *pButton = &g_buttonState[index];

        if (BTN_GetPinForButton((ButtonType)index,
                                &port,
                                &pin) == 0U)
        {
            continue;
        }

        if (GPIO_GetPinValue(port, pin, &rawLevel) != E_OK)
        {
            continue;
        }

        /*
         * Pull-up configuration:
         * LOW  -> pressed
         * HIGH -> released
         */
        rawState = (rawLevel == GPIO_LOW)
                   ? BTN_PRESSED
                   : BTN_RELEASED;

        /*
         * Raw state changed.
         * Restart debounce process.
         */
        if (rawState != pButton->lastRawState)
        {
            pButton->lastRawState = rawState;
            pButton->debounceCount = 0U;
            continue;
        }

        /*
         * Same raw state for another 10 ms sample.
         */
        if (pButton->debounceCount < BTN_DEBOUNCE_SAMPLES)
        {
            pButton->debounceCount++;
        }

        /*
         * State is considered stable after the required
         * number of consecutive samples.
         */
        if (pButton->debounceCount >= BTN_DEBOUNCE_SAMPLES)
        {
            if (pButton->currentState != rawState)
            {
                pButton->currentState = rawState;

                /*
                 * Generate an event only on the
                 * pressed transition.
                 */
                if (rawState == BTN_PRESSED)
                {
                    pButton->edgeFlag = 1U;
                }
            }
        }
    }

    return E_OK;
}

STD_ReturnType BTN_GetState(ButtonType button,
                            ButtonStateType *pState)
{
    if (pState == NULL)
    {
        return E_NOK;
    }

    if (button >= BTN_COUNT)
    {
        return E_NOK;
    }

    *pState = g_buttonState[button].currentState;

    return E_OK;
}

STD_ReturnType BTN_WasPressed(ButtonType button,
                              uint8 *pPressed)
{
    if (pPressed == NULL)
    {
        return E_NOK;
    }

    if (button >= BTN_COUNT)
    {
        return E_NOK;
    }

    *pPressed = g_buttonState[button].edgeFlag;

    /*
     * Reading the event consumes it.
     */
    g_buttonState[button].edgeFlag = 0U;

    return E_OK;
}

STD_ReturnType BTN_ClearEdge(ButtonType button)
{
    if (button >= BTN_COUNT)
    {
        return E_NOK;
    }

    g_buttonState[button].edgeFlag = 0U;

    return E_OK;
}