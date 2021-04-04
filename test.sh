#!/bin/sh -x

modname=lcd_driver_mod

# i2cdetect -r -y 2

# sleep 0.1

# insmod $modname.ko

# sleep 0.1

# i2cdetect -r -y 2

# rmmod $modname

# sleep 0.1

# i2cdetect -r -y 2

insmod $modname.ko

sleep 2

echo -n eto govno rabotaet > /dev/lcd

sleep 5

rmmod $modname
