#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "../../LIB/STD_TYPES.h"

/* ==================== LCD API ==================== */

void LCD_Init(void);

void LCD_Clear(void);

void LCD_Home(void);

void LCD_Goto(uint8 Copy_u8Row, uint8 Copy_u8Column);

void LCD_Print(const char *Copy_p8String);

void LCD_PrintNum(uint16 Copy_u16Number);

void LCD_DisplayOn(void);

void LCD_DisplayOff(void);

#endif /* LCD_I2C_H */