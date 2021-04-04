#ifndef _LCD_CDEV_DEF_H_
#define _LCD_CDEV_DEF_H_

/* Use 'k' as magic number */
#define IOC_MAGIC   'k'

#define LCD_CLEAR                   _IO(IOC_MAGIC, 0)
#define LCD_HOME                    _IO(IOC_MAGIC, 1)
#define LCD_GOTOXY                  _IOW(IOC_MAGIC, 2, struct cursor_pos*)
#define LCD_CURSOR                  _IOW(IOC_MAGIC, 3, bool)
#define LCD_CURSOR_BLINK            _IOW(IOC_MAGIC, 4, bool)
#define LCD_SHIFT_DISPLAY_LEFT      _IO(IOC_MAGIC, 5)
#define LCD_SHIFT_DISPLAY_RIGHT     _IO(IOC_MAGIC, 6)
#define LCD_CREATE_CHAR             _IOW(IOC_MAGIC, 7, uint8_t*)
#define LCD_BACKLIGHT               _IOW(IOC_MAGIC, 8, bool)
#define LCD_PUT_CHAR                _IOW(IOC_MAGIC, 9, char)

#define IOC_MAXNR   9

struct cursor_pos {
    uint8_t x;
    uint8_t y;
};

/*_LCD_CDEV_DEF_H_*/
#endif
