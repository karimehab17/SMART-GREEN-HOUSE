#include "lcd_i2c.h"

#include "config.h"
#include "../../MCAL/I2C/i2c_interface.h"

#include <stddef.h>
#include <util/delay.h>


/* =========================================================
 *                  Private Function Prototypes
 * ========================================================= */

static STD_ReturnType LCD_Write(
    uint8 Copy_u8Control,
    uint8 Copy_u8Data
);

static STD_ReturnType LCD_SendCommand(
    uint8 Copy_u8Command
);

static STD_ReturnType LCD_SendData(
    uint8 Copy_u8Data
);


/* =========================================================
 *                       Initialization
 * ========================================================= */

void LCD_Init(void)
{
    (void)I2C_InitMaster(LCD_I2C_FREQUENCY);

    _delay_ms(LCD_INIT_DELAY_MS);

    /*
     * AiP31068 extended initialization
     */

    (void)LCD_SendCommand(0x38U);
    _delay_ms(5U);

    (void)LCD_SendCommand(0x39U);
    _delay_ms(1U);

    (void)LCD_SendCommand(0x14U);
    (void)LCD_SendCommand(0x78U);
    (void)LCD_SendCommand(0x5EU);
    (void)LCD_SendCommand(0x6AU);

    _delay_ms(200U);

    /*
     * Return to normal instruction set.
     */
    (void)LCD_SendCommand(LCD_FUNCTION_SET);

    _delay_ms(1U);

    /*
     * Display ON, cursor OFF, blink OFF.
     */
    (void)LCD_SendCommand(LCD_DISPLAY_ON);

    _delay_ms(1U);

    /*
     * Clear display.
     */
    (void)LCD_SendCommand(LCD_CLEAR_DISPLAY);

    _delay_ms(LCD_CLEAR_DELAY_MS);

    /*
     * Entry mode: increment DDRAM address.
     */
    (void)LCD_SendCommand(LCD_ENTRY_MODE);

    _delay_ms(1U);
}


/* =========================================================
 *                          Clear
 * ========================================================= */

void LCD_Clear(void)
{
    (void)LCD_SendCommand(LCD_CLEAR_DISPLAY);

    _delay_ms(LCD_CLEAR_DELAY_MS);
}


/* =========================================================
 *                           Home
 * ========================================================= */

void LCD_Home(void)
{
    (void)LCD_SendCommand(0x02U);

    _delay_ms(2U);
}


/* =========================================================
 *                       Cursor Position
 * ========================================================= */

void LCD_Goto(uint8 Copy_u8Row, uint8 Copy_u8Column)
{
    uint8 Local_u8Address;

    if (Copy_u8Column > 15U)
    {
        Copy_u8Column = 15U;
    }

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


/* =========================================================
 *                           Print
 * ========================================================= */

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


/* =========================================================
 *                       Print Number
 * ========================================================= */

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
            (uint8)Local_acBuffer[Local_u8Index]
        );
    }
}


/* =========================================================
 *                     Display Control
 * ========================================================= */

void LCD_DisplayOn(void)
{
    (void)LCD_SendCommand(LCD_DISPLAY_ON);
}


void LCD_DisplayOff(void)
{
    (void)LCD_SendCommand(0x08U);
}


/* =========================================================
 *                    I2C Write Transaction
 * ========================================================= */

static STD_ReturnType LCD_Write(
    uint8 Copy_u8Control,
    uint8 Copy_u8Data
)
{
    STD_ReturnType Local_u8Status;

    /*
     * START
     */
    Local_u8Status = I2C_SendStart();

    if (Local_u8Status != E_OK)
    {
        return E_NOK;
    }

    /*
     * SLA + WRITE
     */
    Local_u8Status =
        I2C_SendSlaveAddressWithWrite(LCD_I2C_ADDRESS);

    if (Local_u8Status != E_OK)
    {
        I2C_SendStop();

        return E_NOK;
    }

    /*
     * AiP31068 control byte
     *
     * 00h = command
     * 40h = data
     */
    Local_u8Status =
        I2C_SendByte(Copy_u8Control);

    if (Local_u8Status != E_OK)
    {
        I2C_SendStop();

        return E_NOK;
    }

    /*
     * Command / Data byte
     */
    Local_u8Status =
        I2C_SendByte(Copy_u8Data);

    I2C_SendStop();

    return Local_u8Status;
}


/* =========================================================
 *                        Command
 * ========================================================= */

static STD_ReturnType LCD_SendCommand(
    uint8 Copy_u8Command
)
{
    return LCD_Write(
        LCD_COMMAND_CONTROL,
        Copy_u8Command
    );
}


/* =========================================================
 *                           Data
 * ========================================================= */

static STD_ReturnType LCD_SendData(
    uint8 Copy_u8Data
)
{
    return LCD_Write(
        LCD_DATA_CONTROL,
        Copy_u8Data
    );
}