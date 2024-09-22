#include "LiquidCrystal_I2C.h"
#include <string.h>

extern void delay_us (uint16_t us); //for microseconds delay implementation

static void expanderWrite(LiquidCrystal_I2C* lcd, uint8_t _data);
static void write4bits(LiquidCrystal_I2C* lcd, uint8_t);
static void pulseEnable(LiquidCrystal_I2C* lcd, uint8_t _data);
static void send(LiquidCrystal_I2C* lcd, uint8_t value, uint8_t mode);


void InitLiquidCrystal_I2C(LiquidCrystal_I2C* lcd, I2C_HandleTypeDef *hi2c, uint8_t lcd_Addr, uint8_t lcd_rows, uint8_t lcd_cols)
{
	lcd->_Addr = lcd_Addr;
	lcd->_cols = lcd_cols;
	lcd->_rows = lcd_rows;
	lcd-> _backlightval = LCD_NOBACKLIGHT;
	lcd->_hi2c = hi2c;
	lcd->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin(lcd, lcd->_rows, lcd->_cols);
}

void begin(LiquidCrystal_I2C* lcd, uint8_t lines, uint8_t cols)
{
	uint8_t dotsize = DOTSIZE;

	if (lines > 1)
	{
		lcd->_displayfunction |= LCD_2LINE;
	}
	lcd->_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1))
	{
		lcd->_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	HAL_Delay(50);

	// Now we pull both RS and R/W low to begin commands
	expanderWrite(lcd, lcd->_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	HAL_Delay(1000);

	// we start in 8bit mode, try to set 4 bit mode
	write4bits(lcd, 0x03 << 4);
//	delay_us(4500); // wait min 4.1ms
	HAL_Delay(5);

	// second try
	write4bits(lcd, 0x03 << 4);
//	delay_us(4500); // wait min 4.1ms
	HAL_Delay(5);

	// third go!
	write4bits(lcd, 0x03 << 4);
//	delay_us(150);
	HAL_Delay(1);

	// finally, set to 4-bit interface
	write4bits(lcd, 0x02 << 4);

	// set # lines, font size, etc.
	command(lcd, LCD_FUNCTIONSET | lcd->_displayfunction);

	// turn the display on with no cursor or blinking default
	lcd->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display(lcd);

	// clear it off
	clear(lcd);

	// Initialize to default text direction (for roman languages)
	lcd->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);

	home(lcd);
}

void clear(LiquidCrystal_I2C* lcd)
{
	command(lcd, LCD_CLEARDISPLAY);// clear display, set cursor position to zero
//	delay_us(2000);  // this command takes a long time!
	HAL_Delay(2);
}

void home(LiquidCrystal_I2C* lcd)
{
	command(lcd, LCD_RETURNHOME);  // set cursor position to zero
//	delay_us(2000);  // this command takes a long time!
	HAL_Delay(2);
}

/********** high level commands, for the user! */

void setCursor(LiquidCrystal_I2C* lcd, uint8_t row, uint8_t col)
{
	int row_offsets[] = { 0x00, 0x40, 0x00 + lcd->_cols, 0x40 + lcd->_cols};
	if ( row > lcd->_numlines ) {
		row = lcd->_numlines-1;    // we count rows starting w/0
	}
	command(lcd, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (quickly)
void noDisplay(LiquidCrystal_I2C* lcd)
{
	lcd->_displaycontrol &= ~LCD_DISPLAYON;
	command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void display(LiquidCrystal_I2C* lcd)
{
	lcd->_displaycontrol |= LCD_DISPLAYON;
	command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

// Turns the underline cursor on/off
void noCursor(LiquidCrystal_I2C* lcd)
{
	lcd->_displaycontrol &= ~LCD_CURSORON;
	command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void cursor(LiquidCrystal_I2C* lcd)
{
	lcd->_displaycontrol |= LCD_CURSORON;
	command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

// Turn on and off the blinking cursor
void noBlink(LiquidCrystal_I2C* lcd)
{
	lcd->_displaycontrol &= ~LCD_BLINKON;
	command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

void blink(LiquidCrystal_I2C* lcd)
{
	lcd->_displaycontrol |= LCD_BLINKON;
	command(lcd, LCD_DISPLAYCONTROL | lcd->_displaycontrol);
}

// These commands scroll the display without changing the RAM
void scrollDisplayLeft(LiquidCrystal_I2C* lcd)
{
	command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void scrollDisplayRight(LiquidCrystal_I2C* lcd)
{
	command(lcd, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void leftToRight(LiquidCrystal_I2C* lcd)
{
	lcd->_displaymode |= LCD_ENTRYLEFT;
	command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This is for text that flows Right to Left
void rightToLeft(LiquidCrystal_I2C* lcd)
{
	lcd->_displaymode &= ~LCD_ENTRYLEFT;
	command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This will 'right justify' text from the cursor
void autoscroll(LiquidCrystal_I2C* lcd)
{
	lcd->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// This will 'left justify' text from the cursor
void noAutoscroll(LiquidCrystal_I2C* lcd)
{
	lcd->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(lcd, LCD_ENTRYMODESET | lcd->_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void createChar(LiquidCrystal_I2C* lcd, uint8_t location, uint8_t charmap[])
{
	location &= 0x7; // we only have 8 locations 0-7
	command(lcd, LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(lcd, charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void noBacklight(LiquidCrystal_I2C* lcd)
{
	lcd->_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(lcd, 0);
}

void backlight(LiquidCrystal_I2C* lcd)
{
	lcd->_backlightval=LCD_BACKLIGHT;
	expanderWrite(lcd, 0);
}

void lcdPrint(LiquidCrystal_I2C* lcd, const char str[])
{
	if (str == NULL)
		return;
	size_t strSize = strlen(str);
	for (size_t i = 0; i < strSize; i++)
	{
		write(lcd , (uint8_t) str[i]);
	}
}

/*********** mid level commands, for sending data/cmds */

void command(LiquidCrystal_I2C* lcd, uint8_t value)
{
	send(lcd, value, 0);
}

void write(LiquidCrystal_I2C* lcd, uint8_t value)
{
	send(lcd, value, Rs);
}

/************ low level data pushing commands **********/

static void expanderWrite(LiquidCrystal_I2C* lcd, uint8_t _data)
{
	uint8_t tx_buffer = _data | lcd->_backlightval;
	HAL_I2C_Master_Transmit(lcd->_hi2c, lcd->_Addr, &tx_buffer, 1, 1000);
}

static void pulseEnable(LiquidCrystal_I2C* lcd, uint8_t _data)
{
	expanderWrite(lcd, _data | En);
//	delay_us(1);
	HAL_Delay(1);
	expanderWrite(lcd, _data & ~En);
//	delay_us(50);
	HAL_Delay(5);
}

static void write4bits(LiquidCrystal_I2C* lcd, uint8_t _data)
{
	expanderWrite(lcd, _data);
	pulseEnable(lcd, _data);
}

// write either command or data
static void send(LiquidCrystal_I2C* lcd, uint8_t value, uint8_t mode)
{
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    write4bits(lcd, (highnib)|mode);
	write4bits(lcd, (lownib)|mode);
}
