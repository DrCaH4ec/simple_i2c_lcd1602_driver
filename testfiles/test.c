#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#include "../lcd_cdev_def.h"

#define FILENAME "/dev/lcd"

#define SYM 1


static uint8_t customChar_1[] = {
    0b00000,
    0b01010,
    0b01010,
    0b01010,
    0b00000,
    0b10001,
    0b01110,
    0b00000,
    0x01
};

static uint8_t customChar_2[] = {
    0b00000,
    0b01010,
    0b01010,
    0b01010,
    0b01110,
    0b10001,
    0b10001,
    0b01110,
    0x02
};

struct cursor_pos pos;

char buf[] = "hyinya";
uint8_t count_buf;
const char *end_of_init = "gavno inited\2";
uint8_t count_end_of_init;

int main(void)
{
    uint8_t j = 0, i = 0;
    int fd = open(FILENAME, O_WRONLY);

    count_buf = strlen(buf);
    count_end_of_init = strlen(end_of_init);

    if(fd == -1) {
        printf("Err with '%s'\n", FILENAME);
        exit(-1);
    } else {
        printf("File '%s' with %d was opened\n", FILENAME, fd);
    }

    ioctl(fd, LCD_CLEAR);
    ioctl(fd, LCD_BACKLIGHT, true);
    ioctl(fd, LCD_CURSOR, true);

    for(i = 0; i < 2; i++) {
        for(j = 0; j < 16; j++) {
            pos.x = j;
            pos.y = i;
            ioctl(fd, LCD_GOTOXY, &pos);
            usleep(200000);
        }
        ioctl(fd, LCD_CURSOR_BLINK, true);
    }

    ioctl(fd, LCD_HOME);

    sleep(1);
    ioctl(fd, LCD_CURSOR_BLINK, false);
    ioctl(fd, LCD_CURSOR, true);
    sleep(1);
    ioctl(fd, LCD_CURSOR, false);

    ioctl(fd, LCD_CREATE_CHAR, customChar_1);
    ioctl(fd, LCD_CREATE_CHAR, customChar_2);
    write(fd, buf, count_buf);


    pos.x = strlen(buf);
    pos.y = 0;
    ioctl(fd, LCD_GOTOXY, &pos);
    ioctl(fd, LCD_PUT_CHAR, 1);
    ioctl(fd, LCD_PUT_CHAR, 2);

    sleep(1);

    for(i = 0,j = 0; j < 5; j++) {
        while((i++)+strlen(buf)+2 < 16) {
            ioctl(fd, LCD_SHIFT_DISPLAY_RIGHT);
            usleep(50000);
        }

        while((--i)>0){
            ioctl(fd, LCD_SHIFT_DISPLAY_LEFT);
            usleep(50000);
        }
    }

    ioctl(fd, LCD_CLEAR);

    for(j = 0; j < 5; j++) {
        ioctl(fd, LCD_BACKLIGHT, true);
        usleep(500000);
        ioctl(fd, LCD_BACKLIGHT, false);
        usleep(500000);
    }  

    ioctl(fd, LCD_BACKLIGHT, true);
    ioctl(fd, LCD_HOME);
    write(fd, end_of_init, count_end_of_init);

    if(close(fd) == -1) {
        printf("Err with '%s'\n", FILENAME);
        exit(-1);
    } else {
        printf("File '%s' was closed\n", FILENAME);
    }

    exit(0);
}
