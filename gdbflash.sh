#!/bin/bash

if [ $# -ne 2 ]; then
    echo "Please enter two arguments (Project name and Port number)"
    exit 1
fi


project=$1
port=$2

if [ -d $project ]; then
	if [ ${project: -1} == '/' ]; then 
		project=${project::-1}
		cd $project/build
	else
		cd $project/build
	fi
else
   echo "$project is not a valid project"
   exit 1
fi

arm-none-eabi-gdb $project.elf -ex "target extended-remote /dev/ttyACM$port" -ex 'monitor tpwr enable' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load'
