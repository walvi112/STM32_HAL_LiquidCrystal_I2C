#ifndef INC_LIQUIDCRYSTAL_I2C_H_
#define INC_LIQUIDCRYSTAL_I2C_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifndef DOTSIZE
#define DOTSIZE	LCD_5x8DOTS
#endif

typedef struct LiquidCrystal_I2C
{
  uint8_t _Addr;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _backlightval;
  I2C_HandleTypeDef *_hi2c;
}LiquidCrystal_I2C;


// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En (0x01 << 2)  // Enable bit
#define Rw (0x01 << 1)  // Read/Write bit
#define Rs (0x01 << 0)  // Register select bit

void InitLiquidCrystal_I2C(LiquidCrystal_I2C* lcd, I2C_HandleTypeDef *hi2c, uint8_t lcd_Addr,uint8_t lcd_rows, uint8_t lcd_cols);
void clear(LiquidCrystal_I2C* lcd);
void home(LiquidCrystal_I2C* lcd);

void noDisplay(LiquidCrystal_I2C* lcd);
void display(LiquidCrystal_I2C* lcd);

void noBlink(LiquidCrystal_I2C* lcd);
void blink(LiquidCrystal_I2C* lcd);

void noCursor(LiquidCrystal_I2C* lcd);
void cursor(LiquidCrystal_I2C* lcd);

void scrollDisplayLeft(LiquidCrystal_I2C* lcd);
void scrollDisplayRight(LiquidCrystal_I2C* lcd);

void leftToRight(LiquidCrystal_I2C* lcd);
void rightToLeft(LiquidCrystal_I2C* lcd);

void noBacklight(LiquidCrystal_I2C* lcd);
void backlight(LiquidCrystal_I2C* lcd);

void autoscroll(LiquidCrystal_I2C* lcd);
void noAutoscroll(LiquidCrystal_I2C* lcd);

void createChar(LiquidCrystal_I2C* lcd, uint8_t location, uint8_t charmap[]);
void setCursor(LiquidCrystal_I2C* lcd, uint8_t row, uint8_t col);

void lcdPrint(LiquidCrystal_I2C* lcd, const char str[]);

void command(LiquidCrystal_I2C* lcd, uint8_t value);
void write(LiquidCrystal_I2C* lcd, uint8_t value);

#endif /* INC_LIQUIDCRYSTAL_I2C_H_ */
