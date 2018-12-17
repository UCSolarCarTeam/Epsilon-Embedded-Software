#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Please enter two arguments (Project name and Port number)"
    exit 1
fi

if [ -d $1 ]; then
   	cd $1/build
else
   echo "$1 is not a valid project"
   exit 1
fi

arm-none-eabi-gdb $1.elf -ex "target extended-remote /dev/ttyACM$2" -ex 'monitor tpwr enable' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load'
