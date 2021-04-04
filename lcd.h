#ifndef _LCD_H_
#define _LCD_H_

//-----------------------------------------------------------------------------
#include <linux/module.h> // required by all modules
#include <linux/kernel.h> // required for sysinfo
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define ADDR    0x27

#define NUM_OF_COLS 16
#define NUM_OF_LINES 2
#define NUM_OF_CUSTOM_CHAR  7
#define DOTS_PER_COL    8

//-----------------------------------------------------------------------------
#define HIGH    1
#define LOW     0
//-----------------------------------------------------------------------------
#define ON      1
#define OFF     0
//-----------------------------------------------------------------------------
#define CLEAR_DISPLAY               (1<<0)
#define RETURN_HOME                 (1<<1)
#define ENTRY_MODE_SET              (1<<2)
#define DISPLAY_ON_OFF_CONTROL      (1<<3)
#define CURSOR_OR_DISPLAY_SHIFT     (1<<4)
#define FUNCTION_SET                (1<<5)
#define SET_CGRAM_ADDRES            (1<<6)
#define SET_DDRAM_ADDRES            (1<<7)

// Entry mode set
#define SH                          (1<<0)
#define I_D                         (1<<1)

// Display ON/OFF control
#define B                           (1<<0)
#define C                           (1<<1)
#define D                           (1<<2)

// Cursor or Display Shift
#define R_L                         (1<<2)
#define S_C                         (1<<3)

// Function set
#define F                           (1<<2)
#define N                           (1<<3)
#define DL                          (1<<4)

#define LCD_RS                      (1<<0)
#define LCD_RW                      (1<<1)
#define LCD_E                       (1<<2)
#define LCD_LED                     (1<<3)
#define LCD_D4                      (1<<4)
#define LCD_D5                      (1<<5)
#define LCD_D6                      (1<<6)
#define LCD_D7                      (1<<7)

//-----------------------------------------------------------------------------

/**
 * This function is used for initialisation of display. It initialises lcd in
 * 4-bit mode.
 * 
 * @param lcd        pointer to struct with pins to which LCD is connected
 * @param NumOfLines number of lines of current LCD
 */
void lcd_init(uint8_t NumOfLines);

/**
 * Clear display and set cursor to position (0, 0)
 * @param lcd pointer to struct with pins to which LCD is connected
 */
void lcd_clear(void);

/**
 * Set cursor to position (0, 0) without changing data on display
 * @param lcd pointer to struct with pins to which LCD is connected
 */
void lcd_home(void);

/**
 * Set cursor to position (x, y) without changing data on display
 * @param lcd pointer to struct with pins to which LCD is connected
 * @param x   new horisontal position of cursor on display
 * @param y   new vertical position of cursor on display
 */
void lcd_gotoxy(uint8_t x, uint8_t y);

/**
 * Print a string to display
 * @param lcd pointer to struct with pins to which LCD is connected
 * @param data pointer to first element of string which will be
 * printed do display
 */
void lcd_puts(char data[]);

/**
 * Print only 1 character to display
 * @param lcd  pointer to struct with pins to which LCD is connected
 * @param data ASCII code of character or addres of custom character
 * in CGRAM
 */
void lcd_put_char(char data);

/**
 * Makes corsor visible on display
 * @param lcd  pointer to struct with pins to which LCD is connected
 * @param mode if mode = 1 cursor is visible, else cursor is invisible
 */
void lcd_cursor(bool mode);

/**
 * Makes corsor blinking on display
 * @param lcd  pointer to struct with pins to which LCD is connected
 * @param mode if mode = 1 cursor is blinking, else cursor isn't blinking
 */
void lcd_cursor_blink(bool mode);

/**
 * Shift all symbols on display on 1 position to left
 * @param lcd pointer to struct with pins to which LCD is connected
 */
void lcd_shift_display_left(void);

/**
 * Shift all symbols on display on 1 position to right
 * @param lcd pointer to struct with pins to which LCD is connected
 */
void lcd_shift_display_right(void);

/**
 * Write to CGRAM custom character with size 5x8 dots
 * @param lcd      pointer to struct with pins to which LCD is connected
 * @param data     pointer to array with definition of new character
 * @param location addres of new character in CGRAM(can be from 0 to 7)
 */
void lcd_create_char(uint8_t data[], uint8_t location);

/**
 * Turn on or turn off a backlight
 * @param mode ON or OFF
 */
void lcd_backlight(bool mode);

/*_LCD_H_*/
#endif
