#!/bin/bash




#---------------------------------------------------------
# Parameters of the script
#---------------------------------------------------------
	cityName=$1
	serverName=$2
	serverSpec=$3
	sendProjectToHost=$4
	experienceName=$5


#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
	parameterFile_prefix=.parameter
	parameterFile=$parameterFile_prefix-$serverName
	exec=setRemoteAllocator_slave_son.sh
	output=../statistic/$serverSpec.stdout


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
	echo "Set Local Allocator script"
	hostname
	echo "City                : "$cityName
	echo "Server              : "$serverName
	echo "Server spec         : "$serverSpec
	echo "Experience name     : "$experienceName
	echo "Current directory   : "`pwd`
	echo "Parameter file      : "$parameterFile
	echo "----------------------------------------------"


	# Write the parameters in the param file
	echo "$*" > $parameterFile
	./$exec > $output
	rm -f $parameterFile

	exit
