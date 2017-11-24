#!/bin/bash






#---------------------------------------------------------
# Parameters of the script
#---------------------------------------------------------
	cityName=$1
	serverSpec=$2
	resDir=$3


#---------------------------------------------------------
# Main script
#---------------------------------------------------------
	echo
	echo
	echo
	echo
	echo
	echo
	echo
	echo
	echo "----------------------------------------------"
	echo "Get remote result"
	hostname
	echo "City                : "$cityName
	echo "Server spec         : "$serverSpec
	echo "resultDirectory     : "$resDir
	echo "Current directory   : "`pwd`
	echo "----------------------------------------------"

	mkdir $resDir
	echo "$cityName:internship_mem_alloc_project/statistic/$serverSpec* ../statistic/$resDir/"
scp -r $cityName:internship_mem_alloc_project/statistic/*$serverSpec* ../statistic/$resDir/

