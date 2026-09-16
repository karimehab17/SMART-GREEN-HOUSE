#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "../../LIB/STD_TYPES.h"
#include "config.h"

/* ==================== LCD Public API ==================== */

void LCD_Init(void);

void LCD_Clear(void);

void LCD_Goto(uint8 Copy_u8Row, uint8 Copy_u8Column);

void LCD_Print(const char *Copy_p8String);

void LCD_PrintNum(uint16 Copy_u16Number);

#endif /* LCD_I2C_H */