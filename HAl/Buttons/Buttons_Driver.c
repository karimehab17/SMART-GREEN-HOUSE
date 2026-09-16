#include "Buttons_Driver.h"

#include "../../MCAL/GPIO/GPIO_interface.h"
#include "config.h"

#include <stddef.h>

typedef struct
{
    ButtonStateType currentState;
    ButtonStateType lastRawState;
    uint8 debounceCount;
    uint8 edgeFlag;
} ButtonDebounceType;

static ButtonDebounceType g_buttonState[BTN_COUNT];

static uint8 BTN_GetPin(ButtonType button,
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
            *pPort = BUTTON_RESET_PORT;
            *pPin = BUTTON_RESET_PIN;
            break;

        case BTN_MODE:
            *pPort = BUTTON_MODE_PORT;
            *pPin = BUTTON_MODE_PIN;
            break;

        case BTN_SAVE:
            *pPort = BUTTON_SAVE_PORT;
            *pPin = BUTTON_SAVE_PIN;
            break;

        default:
            return 0U;
    }

    return 1U;
}

static ButtonStateType BTN_ReadRaw(ButtonType button)
{
    uint8 port = 0U;
    uint8 pin = 0U;
    uint8 level = GPIO_HIGH;

    if (BTN_GetPin(button, &port, &pin) == 0U)
    {
        return BTN_RELEASED;
    }

    if (GPIO_GetPinValue(port, pin, &level) != E_OK)
    {
        return BTN_RELEASED;
    }

    return (level == GPIO_LOW)
           ? BTN_PRESSED
           : BTN_RELEASED;
}

static void BTN_ResetState(ButtonType button)
{
    g_buttonState[button].currentState = BTN_RELEASED;
    g_buttonState[button].lastRawState = BTN_RELEASED;
    g_buttonState[button].debounceCount = 0U;
    g_buttonState[button].edgeFlag = 0U;
}

static void BTN_UpdateDebounce(ButtonType button,
                               ButtonStateType rawState)
{
    ButtonDebounceType *pButton = &g_buttonState[button];

    if (rawState != pButton->lastRawState)
    {
        pButton->lastRawState = rawState;
        pButton->debounceCount = 0U;
        return;
    }

    if (pButton->debounceCount < BUTTON_DEBOUNCE_SAMPLES)
    {
        pButton->debounceCount++;
    }

    if (pButton->debounceCount >= BUTTON_DEBOUNCE_SAMPLES)
    {
        if (pButton->currentState != rawState)
        {
            pButton->currentState = rawState;

            if (rawState == BTN_PRESSED)
            {
                pButton->edgeFlag = 1U;
            }
        }
    }
}

STD_ReturnType BTN_Init(void)
{
    uint8 index;

    for (index = 0U; index < BTN_COUNT; index++)
    {
        BTN_ResetState((ButtonType)index);
    }

    if (GPIO_SetPinDirection(BUTTON_RESET_PORT,
                             BUTTON_RESET_PIN,
                             GPIO_INPUT_PULLUP) != E_OK)
    {
        return E_NOK;
    }

    if (GPIO_SetPinDirection(BUTTON_MODE_PORT,
                             BUTTON_MODE_PIN,
                             GPIO_INPUT_PULLUP) != E_OK)
    {
        return E_NOK;
    }

    return GPIO_SetPinDirection(BUTTON_SAVE_PORT,
                                BUTTON_SAVE_PIN,
                                GPIO_INPUT_PULLUP);
}

STD_ReturnType BTN_Poll(void)
{
    uint8 index;

    for (index = 0U; index < BTN_COUNT; index++)
    {
        ButtonStateType rawState;

        rawState = BTN_ReadRaw((ButtonType)index);
        BTN_UpdateDebounce((ButtonType)index, rawState);
    }

    return E_OK;
}

STD_ReturnType BTN_GetState(ButtonType button,
                            ButtonStateType *pState)
{
    if ((button >= BTN_COUNT) || (pState == NULL))
    {
        return E_NOK;
    }

    *pState = g_buttonState[button].currentState;

    return E_OK;
}

STD_ReturnType BTN_WasPressed(ButtonType button,
                              uint8 *pPressed)
{
    if ((button >= BTN_COUNT) || (pPressed == NULL))
    {
        return E_NOK;
    }

    *pPressed = g_buttonState[button].edgeFlag;
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