#include "lcd_i2c.h"

int main(void)
{
    LCD_Init();

    LCD_Goto(0u, 0u);
    LCD_Print("Karim");

    LCD_Goto(1u, 0u);
    LCD_Print("Ehab");

    while (1)
    {
    }

    return 0;
}