#include "lcd_i2c.h"

#include "../../MCAL/I2C/i2c_interface.h"

#include <stddef.h>
#include <util/delay.h>


/* ==================== Private Functions ==================== */

static STD_ReturnType LCD_Write(
    uint8 Copy_u8Control,
    uint8 Copy_u8Data);

static STD_ReturnType LCD_SendCommand(
    uint8 Copy_u8Command);

static STD_ReturnType LCD_SendData(
    uint8 Copy_u8Data);


/* ==================== Initialization ==================== */

void LCD_Init(void)
{
    I2C_InitMaster(LCD_I2C_FREQUENCY);

    _delay_ms(LCD_INIT_DELAY_MS);

    (void)LCD_SendCommand(LCD_FUNCTION_SET);
    (void)LCD_SendCommand(LCD_DISPLAY_ON);
    (void)LCD_SendCommand(LCD_CLEAR_DISPLAY);

    _delay_ms(LCD_CLEAR_DELAY_MS);

    (void)LCD_SendCommand(LCD_ENTRY_MODE);
}


/* ==================== Clear ==================== */

void LCD_Clear(void)
{
    (void)LCD_SendCommand(LCD_CLEAR_DISPLAY);

    _delay_ms(LCD_CLEAR_DELAY_MS);
}


/* ==================== Position ==================== */

void LCD_Goto(
    uint8 Copy_u8Row,
    uint8 Copy_u8Column)
{
    uint8 Local_u8Address;

    if (Copy_u8Row == 0U)
    {
        Local_u8Address =
            (uint8)(LCD_LINE0_ADDRESS + Copy_u8Column);
    }
    else
    {
        Local_u8Address =
            (uint8)(LCD_LINE1_ADDRESS + Copy_u8Column);
    }

    (void)LCD_SendCommand(Local_u8Address);
}


/* ==================== String ==================== */

void LCD_Print(const char *Copy_p8String)
{
    if (Copy_p8String == NULL)
    {
        return;
    }

    while (*Copy_p8String != '\0')
    {
        (void)LCD_SendData((uint8)*Copy_p8String);
        Copy_p8String++;
    }
}


/* ==================== Number ==================== */

void LCD_PrintNum(uint16 Copy_u16Number)
{
    char Local_acBuffer[5];
    uint8 Local_u8Index = 0U;

    if (Copy_u16Number == 0U)
    {
        (void)LCD_SendData((uint8)'0');
        return;
    }

    while (Copy_u16Number > 0U)
    {
        Local_acBuffer[Local_u8Index] =
            (char)('0' + (Copy_u16Number % 10U));

        Copy_u16Number /= 10U;
        Local_u8Index++;
    }

    while (Local_u8Index > 0U)
    {
        Local_u8Index--;

        (void)LCD_SendData(
            (uint8)Local_acBuffer[Local_u8Index]);
    }
}


/* ==================== Low-Level Write ==================== */

static STD_ReturnType LCD_Write(
    uint8 Copy_u8Control,
    uint8 Copy_u8Data)
{
    STD_ReturnType Local_u8Status;

    Local_u8Status = I2C_SendStart();

    if (Local_u8Status != E_OK)
    {
        return E_NOK;
    }

    Local_u8Status =
        I2C_SendSlaveAddressWithWrite(LCD_I2C_ADDRESS);

    if (Local_u8Status != E_OK)
    {
        (void)I2C_SendStop();
        return E_NOK;
    }

    Local_u8Status =
        I2C_SendByte(Copy_u8Control);

    if (Local_u8Status != E_OK)
    {
        (void)I2C_SendStop();
        return E_NOK;
    }

    Local_u8Status =
        I2C_SendByte(Copy_u8Data);

    (void)I2C_SendStop();

    return Local_u8Status;
}


/* ==================== Command ==================== */

static STD_ReturnType LCD_SendCommand(
    uint8 Copy_u8Command)
{
    return LCD_Write(
        LCD_COMMAND_CONTROL,
        Copy_u8Command);
}


/* ==================== Data ==================== */

static STD_ReturnType LCD_SendData(
    uint8 Copy_u8Data)
{
    return LCD_Write(
        LCD_DATA_CONTROL,
        Copy_u8Data);
}