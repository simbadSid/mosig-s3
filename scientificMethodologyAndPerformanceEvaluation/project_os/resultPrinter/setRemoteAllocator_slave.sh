#!/bin/bash




#---------------------------------------------------------
# Parameters of the script
#---------------------------------------------------------
	cityName=$1
	serverName=$2
	serverSpec=$3
	sendProjectToHost=$4
	experienceName=$5
	execTest=$6
	backoff=$7



#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
	parameterFile_prefix=.parameter
	parameterFile=$parameterFile_prefix-$serverName
	execPath=internship_mem_alloc_project/resultPrinter
	exec=setRemoteAllocator_slave_son.sh
	output=internship_mem_alloc_project/statistic/$serverSpec.stdout
	outputError=internship_mem_alloc_project/statistic/$serverSpec.stderr


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
	echo "Slave script"
	hostname
	echo "City                : "$cityName
	echo "Server              : "$serverName
	echo "Server spec         : "$serverSpec
	echo "Send project to host: "$sendProjectToHost
	echo "Experience name     : "$experienceName
	echo "Executable          : "$execTest
	echo "Backoff value       : "$backoff
	echo "Current directory   : "`pwd`
	echo "Parameter file      : "$parameterFile
	echo "----------------------------------------------"

	if [ $sendProjectToHost == "True" ]; then
		tar xvf internship_mem_alloc_project.tar
####		rm -f internship_mem_alloc_project.tar
	fi

	# Write the parameters in the param file
	echo "$*" > $parameterFile
	cp $parameterFile $execPath
	rm -f $parameterFile

	oarsub -p "cluster='$serverName'" -n $experienceName-$serverName -l nodes=1,walltime=4 -O $output -E $outputError $execPath/$exec

	exit
