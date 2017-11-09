#!/bin/sh

chmod 740 /opt/opt1002/bin/opt1002_main 

insmod /opt/opt1002/drvs/device_idx_drv.ko
insmod /opt/opt1002/drvs/ds1302_drv.ko
insmod /opt/opt1002/drvs/device_led_drv.ko
insmod /opt/opt1002/drvs/device_pwr_drv.ko

./opt/opt1002/bin/opt1002_main &
./opt/opt1002/bin/test &

cd /
mount -o rw,remount /

