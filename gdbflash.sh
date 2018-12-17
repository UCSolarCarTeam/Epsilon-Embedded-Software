#!/bin/bash

cd $1/build

arm-none-eabi-gdb $1.elf -ex "target extended-remote /dev/ttyACM$2" -ex 'monitor tpwr enable' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load'
