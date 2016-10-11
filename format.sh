#!/bin/bash -e

if [ $# -eq 0 ]; then 
	echo "Pass directory to format as argument"
	echo "Example: ./format.sh Lights"
	exit 1
fi

astyle "$1/*.h" "$1/*.c" -r --options=astylerc
find $1 -name "*.orig" -delete
