#include "../../LIB/STD_TYPES.h"
#include "../../LIB/Math.h"

#include "GPIO_interface.h"
#include "GPIO_private.h"


/* =========================================================
 *                  Private Helper Functions
 * ========================================================= */

static STD_ReturnType GPIO_IsValidPort(uint8 Copy_u8Port)
{
    if (Copy_u8Port <= GPIO_PORTD)
    {
        return E_OK;
    }

    return E_port_not_valid;
}


static STD_ReturnType GPIO_IsValidPin(uint8 Copy_u8Pin)
{
    if (Copy_u8Pin <= GPIO_PIN7)
    {
        return E_OK;
    }

    return E_pin_not_valid;
}


static volatile uint8* GPIO_GetPortRegister(uint8 Copy_u8Port)
{
    switch (Copy_u8Port)
    {
        case GPIO_PORTA:
            return &GPIO_PORTA_REG;

        case GPIO_PORTB:
            return &GPIO_PORTB_REG;

        case GPIO_PORTC:
            return &GPIO_PORTC_REG;

        case GPIO_PORTD:
            return &GPIO_PORTD_REG;

        default:
            return (volatile uint8*)0;
    }
}


static volatile uint8* GPIO_GetDDRRegister(uint8 Copy_u8Port)
{
    switch (Copy_u8Port)
    {
        case GPIO_PORTA:
            return &GPIO_DDRA_REG;

        case GPIO_PORTB:
            return &GPIO_DDRB_REG;

        case GPIO_PORTC:
            return &GPIO_DDRC_REG;

        case GPIO_PORTD:
            return &GPIO_DDRD_REG;

        default:
            return (volatile uint8*)0;
    }
}


static volatile uint8* GPIO_GetPINRegister(uint8 Copy_u8Port)
{
    switch (Copy_u8Port)
    {
        case GPIO_PORTA:
            return &GPIO_PINA_REG;

        case GPIO_PORTB:
            return &GPIO_PINB_REG;

        case GPIO_PORTC:
            return &GPIO_PINC_REG;

        case GPIO_PORTD:
            return &GPIO_PIND_REG;

        default:
            return (volatile uint8*)0;
    }
}


/* =========================================================
 *                  Pin Direction
 * ========================================================= */

STD_ReturnType GPIO_SetPinDirection(uint8 Copy_u8Port,uint8 Copy_u8Pin,uint8 Copy_u8Direction)
{
    volatile uint8* Local_pu8DDR;
    volatile uint8* Local_pu8PORT;

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    if (GPIO_IsValidPin(Copy_u8Pin) != E_OK)
    {
        return E_pin_not_valid;
    }

    Local_pu8DDR = GPIO_GetDDRRegister(Copy_u8Port);
    Local_pu8PORT = GPIO_GetPortRegister(Copy_u8Port);

    switch (Copy_u8Direction)
    {
        case GPIO_INPUT:

            CLEAR_BIT(*Local_pu8DDR, Copy_u8Pin);
            CLEAR_BIT(*Local_pu8PORT, Copy_u8Pin);

            break;

        case GPIO_OUTPUT:

            SET_BIT(*Local_pu8DDR, Copy_u8Pin);

            break;

        case GPIO_INPUT_PULLUP:

            CLEAR_BIT(*Local_pu8DDR, Copy_u8Pin);
            SET_BIT(*Local_pu8PORT, Copy_u8Pin);

            break;

        default:

            return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                     Pin Value
 * ========================================================= */

STD_ReturnType GPIO_SetPinValue(uint8 Copy_u8Port,uint8 Copy_u8Pin,uint8 Copy_u8Value)
{
    volatile uint8* Local_pu8PORT;

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    if (GPIO_IsValidPin(Copy_u8Pin) != E_OK)
    {
        return E_pin_not_valid;
    }

    if (Copy_u8Value > GPIO_HIGH)
    {
        return E_NOK;
    }

    Local_pu8PORT = GPIO_GetPortRegister(Copy_u8Port);

    if (Copy_u8Value == GPIO_HIGH)
    {
        SET_BIT(*Local_pu8PORT, Copy_u8Pin);
    }
    else
    {
        CLEAR_BIT(*Local_pu8PORT, Copy_u8Pin);
    }

    return E_OK;
}


STD_ReturnType GPIO_GetPinValue(uint8 Copy_u8Port,uint8 Copy_u8Pin, uint8* Copy_pu8Value)
{
    volatile uint8* Local_pu8PIN;

    if (Copy_pu8Value == (uint8*)0)
    {
        return E_NOK;
    }

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    if (GPIO_IsValidPin(Copy_u8Pin) != E_OK)
    {
        return E_pin_not_valid;
    }

    Local_pu8PIN = GPIO_GetPINRegister(Copy_u8Port);

    *Copy_pu8Value = GET_BIT(*Local_pu8PIN, Copy_u8Pin);

    return E_OK;
}


STD_ReturnType GPIO_TogglePinValue(uint8 Copy_u8Port,uint8 Copy_u8Pin)
{
    volatile uint8* Local_pu8PORT;

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    if (GPIO_IsValidPin(Copy_u8Pin) != E_OK)
    {
        return E_pin_not_valid;
    }

    Local_pu8PORT = GPIO_GetPortRegister(Copy_u8Port);

    TOGGLE_BIT(*Local_pu8PORT, Copy_u8Pin);

    return E_OK;
}


/* =========================================================
 *                     Port Direction
 * ========================================================= */

STD_ReturnType GPIO_SetPortDirection(uint8 Copy_u8Port,uint8 Copy_u8Direction)
{
    volatile uint8* Local_pu8DDR;

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    Local_pu8DDR = GPIO_GetDDRRegister(Copy_u8Port);

    switch (Copy_u8Direction)
    {
        case GPIO_INPUT:

            *Local_pu8DDR = 0x00U;

            break;

        case GPIO_OUTPUT:

            *Local_pu8DDR = 0xFFU;

            break;

        default:

            return E_NOK;
    }

    return E_OK;
}


/* =========================================================
 *                       Port Value
 * ========================================================= */

STD_ReturnType GPIO_SetPortValue(uint8 Copy_u8Port,uint8 Copy_u8Value)
{
    volatile uint8* Local_pu8PORT;

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    Local_pu8PORT = GPIO_GetPortRegister(Copy_u8Port);

    *Local_pu8PORT = Copy_u8Value;

    return E_OK;
}


STD_ReturnType GPIO_GetPortValue(uint8 Copy_u8Port,uint8* Copy_pu8Value)
{
    volatile uint8* Local_pu8PIN;

    if (Copy_pu8Value == (uint8*)0)
    {
        return E_NOK;
    }

    if (GPIO_IsValidPort(Copy_u8Port) != E_OK)
    {
        return E_port_not_valid;
    }

    Local_pu8PIN = GPIO_GetPINRegister(Copy_u8Port);

    *Copy_pu8Value = *Local_pu8PIN;

    return E_OK;
}