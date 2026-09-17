#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H

#include "../../LIB/STD_TYPES.h"


/* =========================================================
 *                         GPIO Ports
 * ========================================================= */

#define GPIO_PORTA    0U
#define GPIO_PORTB    1U
#define GPIO_PORTC    2U
#define GPIO_PORTD    3U


/* =========================================================
 *                         GPIO Pins
 * ========================================================= */

#define GPIO_PIN0     0U
#define GPIO_PIN1     1U
#define GPIO_PIN2     2U
#define GPIO_PIN3     3U
#define GPIO_PIN4     4U
#define GPIO_PIN5     5U
#define GPIO_PIN6     6U
#define GPIO_PIN7     7U


/* =========================================================
 *                      Pin Direction
 * ========================================================= */

#define GPIO_INPUT          0U
#define GPIO_OUTPUT         1U
#define GPIO_INPUT_PULLUP   2U


/* =========================================================
 *                        Pin Value
 * ========================================================= */

#define GPIO_LOW            0U
#define GPIO_HIGH           1U


/* =========================================================
 *                        GPIO API
 * ========================================================= */

STD_ReturnType GPIO_SetPinDirection(
    uint8 Copy_u8Port,
    uint8 Copy_u8Pin,
    uint8 Copy_u8Direction
);

STD_ReturnType GPIO_SetPinValue(
    uint8 Copy_u8Port,
    uint8 Copy_u8Pin,
    uint8 Copy_u8Value
);

STD_ReturnType GPIO_GetPinValue(
    uint8 Copy_u8Port,
    uint8 Copy_u8Pin,
    uint8* Copy_pu8Value
);

STD_ReturnType GPIO_TogglePinValue(
    uint8 Copy_u8Port,
    uint8 Copy_u8Pin
);

STD_ReturnType GPIO_SetPortDirection(
    uint8 Copy_u8Port,
    uint8 Copy_u8Direction
);

STD_ReturnType GPIO_SetPortValue(
    uint8 Copy_u8Port,
    uint8 Copy_u8Value
);

STD_ReturnType GPIO_GetPortValue(
    uint8 Copy_u8Port,
    uint8* Copy_pu8Value
);


#endif /* GPIO_INTERFACE_H */