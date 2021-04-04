// SPDX-License-Identifier: MIT and GPL
#include "lcd.h"

static uint8_t lcd_reg;
extern struct i2c_client *lcd_i2c_client;

//#############################################################################
//#####################---PLATFORM DEPENDENT---################################
//#############################################################################

static inline void ms_delay(uint16_t msecs)
{
	msleep(msecs);
}

static inline void us_delay(uint16_t usecs)
{
	fsleep(usecs);
}

static void _send_byte(uint8_t data)
{
	i2c_smbus_write_byte_data(lcd_i2c_client, 0x00, data);
}

//#############################################################################
//#####################---INTERNAL FUNCTIONS---################################
//#############################################################################

static void _pulse(void)
{
	lcd_reg &= ~LCD_E;

	_send_byte(lcd_reg);

	us_delay(10);

	lcd_reg |= LCD_E;

	_send_byte(lcd_reg);

	us_delay(10);

	lcd_reg &= ~LCD_E;

	_send_byte(lcd_reg);

	us_delay(100);
}

//-----------------------------------------------------------------------------

// part=1 -> high
// part=0 -> low
static void SendHalfByte(uint8_t data, bool part)
{
	lcd_reg &= ~(LCD_D4 | LCD_D5 | LCD_D6 | LCD_D7);

	if (part) {
		lcd_reg = data & (1 << 4) ? lcd_reg | LCD_D4 : lcd_reg;
		lcd_reg = data & (1 << 5) ? lcd_reg | LCD_D5 : lcd_reg;
		lcd_reg = data & (1 << 6) ? lcd_reg | LCD_D6 : lcd_reg;
		lcd_reg = data & (1 << 7) ? lcd_reg | LCD_D7 : lcd_reg;

	} else {
		lcd_reg = data & (1 << 0) ? lcd_reg | LCD_D4 : lcd_reg;
		lcd_reg = data & (1 << 1) ? lcd_reg | LCD_D5 : lcd_reg;
		lcd_reg = data & (1 << 2) ? lcd_reg | LCD_D6 : lcd_reg;
		lcd_reg = data & (1 << 3) ? lcd_reg | LCD_D7 : lcd_reg;
	}

	_pulse();
}

//-----------------------------------------------------------------------------

static void _send_instr(uint8_t data)
{
	lcd_reg &= ~LCD_RS;

	SendHalfByte(data, HIGH);
	SendHalfByte(data, LOW);
}

//-----------------------------------------------------------------------------

static void _send_data(uint8_t data)
{
	lcd_reg |= LCD_RS;

	us_delay(1);

	SendHalfByte(data, HIGH);
	SendHalfByte(data, LOW);

	us_delay(50);
}

//#############################################################################
//#####################---EXTERNAL FUNCTIONS---################################
//#############################################################################

void lcd_backlight(bool mode)
{
	lcd_reg = mode ? lcd_reg | LCD_LED : lcd_reg & (~LCD_LED);
	_send_byte(lcd_reg);
}

//-----------------------------------------------------------------------------

void lcd_clear(void)
{
	_send_instr(CLEAR_DISPLAY);
	ms_delay(2);
}

//-----------------------------------------------------------------------------

void lcd_home(void)
{
	_send_instr(RETURN_HOME);
	ms_delay(2);
}

//-----------------------------------------------------------------------------

void lcd_init(uint8_t NumOfLines)
{
	uint8_t tmp = 0x00;

	ms_delay(50);

	tmp = FUNCTION_SET | DL;

	SendHalfByte(tmp, HIGH);
	ms_delay(5);

	SendHalfByte(tmp, HIGH);
	us_delay(150);

	SendHalfByte(tmp, HIGH);

	us_delay(50);

	tmp = 0x00;
	tmp |= FUNCTION_SET;
	SendHalfByte(tmp, HIGH);

	//2 lines mode set
	//5x8 dots
	tmp = NumOfLines > 1 ? (tmp | N) : tmp;
	_send_instr(tmp);

	_send_instr(tmp);

	tmp = 0x00;
	tmp = DISPLAY_ON_OFF_CONTROL | D;
	_send_instr(tmp);

	tmp = 0x00;
	tmp = ENTRY_MODE_SET | I_D;
	_send_instr(tmp);

	lcd_clear();
}

//-----------------------------------------------------------------------------

void lcd_gotoxy(uint8_t x, uint8_t y)
{
	uint8_t tmp = (0x40 * y + x) | SET_DDRAM_ADDRES;

	_send_instr(tmp);
	us_delay(100);
}

//-----------------------------------------------------------------------------

void lcd_puts(char data[])
{
	uint8_t i = 0;

	while (data[i] != '\0' && i < NUM_OF_COLS) {
		_send_data(data[i]);
		i++;
	}
}

//-----------------------------------------------------------------------------

void lcd_put_char(char data)
{
	_send_data(data);
}

//-----------------------------------------------------------------------------

void lcd_cursor(bool mode)
{
	uint8_t tmp = DISPLAY_ON_OFF_CONTROL | D;

	tmp = mode ? (tmp | C) : tmp;

	_send_instr(tmp);
}

//-----------------------------------------------------------------------------

void lcd_cursor_blink(bool mode)
{
	uint8_t tmp = DISPLAY_ON_OFF_CONTROL | D;

	tmp = mode ? (tmp | B) : tmp;

	_send_instr(tmp);
}

//-----------------------------------------------------------------------------

void lcd_display(bool mode)
{
	uint8_t tmp = DISPLAY_ON_OFF_CONTROL;

	tmp = mode ? (tmp | D) : tmp;

	_send_instr(tmp);
}

//-----------------------------------------------------------------------------

void lcd_shift_display_left(void)
{
	uint8_t tmp = CURSOR_OR_DISPLAY_SHIFT | S_C;

	_send_instr(tmp);
}

//-----------------------------------------------------------------------------

void lcd_shift_display_right(void)
{
	uint8_t tmp = CURSOR_OR_DISPLAY_SHIFT | S_C | R_L;

	_send_instr(tmp);
}

//-----------------------------------------------------------------------------

void lcd_create_char(uint8_t data[], uint8_t location)
{
	uint8_t tmp = SET_CGRAM_ADDRES | (location << 3);
	uint8_t i;

	_send_instr(tmp);

	for (i = 0; i < 8; i++)
		_send_data(data[i]);
}
