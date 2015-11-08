#!/bin/bash

adb push hung.ko /data
adb shell insmod /data/hung.ko
#adb shell "echo 1 > /sys/kernel/debug/hung/mutex"
#adb shell "echo 1 > /sys/kernel/debug/hung/msleep"
#adb shell "echo 1 > /sys/kernel/debug/hung/mdelay"
adb shell rmmod hung
adb shell dmesg | grep hung
