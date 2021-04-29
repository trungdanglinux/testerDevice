
#ifndef __LCD_H__
#define __LCD_H__


void init_LCD();

void LCD_write(unsigned char *text);
void LCD_CursorOn(uint8_t ON);
void LCD_Clear();
void LCD_cursorXY(uint8_t x, uint8_t y);

#endif
