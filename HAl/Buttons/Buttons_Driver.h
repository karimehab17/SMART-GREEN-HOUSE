#ifndef BUTTONS_DRIVER_H
#define BUTTONS_DRIVER_H

#include "STD_TYPES.h"

typedef enum
{
    BTN_RESET = 0,
    BTN_MODE,
    BTN_SAVE,
    BTN_COUNT
} ButtonType;

typedef enum
{
    BTN_RELEASED = 0,
    BTN_PRESSED = 1
} ButtonStateType;

/*
 * Initializes all greenhouse buttons.
 *
 * BTN_RESET -> PD2 / INT0
 * BTN_MODE  -> PD3 / INT1
 * BTN_SAVE  -> PD4
 */
STD_ReturnType BTN_Init(void);

/*
 * Called periodically by the scheduler.
 * Performs button sampling and debounce.
 */
STD_ReturnType BTN_Poll(void);

/*
 * Returns the current stable state of a button.
 */
STD_ReturnType BTN_GetState(ButtonType button,
                            ButtonStateType *pState);

/*
 * Returns 1 if the button generated a new press event.
 * The event is cleared after reading.
 */
STD_ReturnType BTN_WasPressed(ButtonType button,
                              uint8 *pPressed);

/*
 * Clears a pending press event.
 */
STD_ReturnType BTN_ClearEdge(ButtonType button);

#endif /* BUTTONS_DRIVER_H */