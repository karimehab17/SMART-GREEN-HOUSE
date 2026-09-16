#ifndef BUTTONS_DRIVER_H
#define BUTTONS_DRIVER_H

#include "../../LIB/STD_TYPES.h"

typedef enum
{
    BTN_RESET = 0U,
    BTN_MODE,
    BTN_SAVE,
    BTN_COUNT
} ButtonType;

typedef enum
{
    BTN_RELEASED = 0U,
    BTN_PRESSED = 1U
} ButtonStateType;

STD_ReturnType BTN_Init(void);

STD_ReturnType BTN_Poll(void);

STD_ReturnType BTN_GetState(ButtonType button, ButtonStateType *pState);

STD_ReturnType BTN_WasPressed(ButtonType button, uint8 *pPressed);

STD_ReturnType BTN_ClearEdge(ButtonType button);

#endif /* BUTTONS_DRIVER_H */