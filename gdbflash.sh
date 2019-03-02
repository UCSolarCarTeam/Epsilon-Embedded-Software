#!/bin/bash

while getopts "p:b:" args
do
    case $args in
        p) # str1
            project=$OPTARG
            ;;
        b) # str2
            blackMagicPath=$OPTARG
            ;;
        \?)
          echo "Invalid option: -$OPTARG" >&2
          exit 1
          ;;
        :)
          echo "Option -$OPTARG requires an argument." >&2
          exit 1
    esac
done

if [ $OPTIND != 5 ]; then
  echo "Please enter two options (Project name and Port number)"
  echo "Example: ./gdbflash.sh -p EpsilonLights -b /dev/ttyACM0"
  exit
fi

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

arm-none-eabi-gdb $project.elf -ex "target extended-remote $blackMagicPath" -ex 'monitor tpwr enable' -ex 'monitor swdp_scan' -ex 'attach 1' -ex 'load'
