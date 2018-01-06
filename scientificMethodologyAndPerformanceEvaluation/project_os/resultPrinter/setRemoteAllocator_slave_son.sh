#!/bin/bash




#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
#	declare -a execTestCCList=("cc-msqueue")
#	declare -a execTestMPList=("mp-msqueue-1" "mp-msqueue-2" "mp-msqueue-s")
#	declare -a backoffMPList=("0" "2500" "5000" "7500" "10000" "15000")
#	declare -a backoffMPList=("0" "2500" "5000" "7500" "10000" "")
	execPath=internship_mem_alloc_project/resultPrinter
	parameterFile_prefix=.parameter-
	parameterFile=""
	utilDir="../myUtil"

	if [ `pwd` != "*$execPath*" ]; then
#TODO problem ici
		cd $execPath
	fi

	# Parse the input parameters fron the input file
	function findParameterFile()
	{
		for fic in `ls -a`; do
			if [[ $fic != $parameterFile_prefix* ]]; then
				continue
			fi
			serverName=${fic#$parameterFile_prefix}
			if [[ `hostname` == *$serverName* ]]; then
				eval "$1=$fic"
				return
			fi
		done

		echo "****************************"
		echo "Can't find the parameter file in the server: "`hostname`
		echo "****************************"
		exit
	}
	findParameterFile parameterFile

	# Extract the NUMA node list of the machine
	make -C $utilDir mrproper
	make -C $utilDir
	nodeList=`$utilDir/extractNumaArchitecture -nl`


#---------------------------------------------------------
# Parameters of the script (input read from file)
#---------------------------------------------------------
oldIFS="$IFS"
IFS=' '
	param=`cat $parameterFile`
	read -r -a paramTab <<< "$param"
#TODO	rm -f $parameterFile

	cityName=${paramTab[0]}
	serverName=${paramTab[1]}
	serverSpec=${paramTab[2]}
	sendProjectToHost=${paramTab[3]}
	experienceName=${paramTab[4]}
	execTest=${paramTab[5]}
	backoff=${paramTab[6]}

IFS="$oldIFS"


#---------------------------------------------------------
# Main script
#---------------------------------------------------------
	echo "----------------------------------------------"
	echo "Slave son script"
	hostname
	echo "City                : "$cityName
	echo "Server              : "$serverName
	echo "Server spec         : "$serverSpec
	echo "Send project to host: "$sendProjectToHost
	echo "Experience name     : "$experienceName
	echo "Current directory   : "`pwd`
	echo "Parameter file      : "$parameterFile
	echo "----------------------------------------------"

	# Forbid the transactional memory: usage of atomic memory instruction for synchronization
	python printer.py -exec=$execTest -noPlot -backoff=$backoff -noTransactionalMemory -statDir=$serverSpec-noTransactionalMemory-$backoff-$execTest

	# Default allocation policy: First touch: Allocations done on the node where it is ordered
	python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-noOption-$backoff-$execTest

	# Change the kernel allocation policy: all the allocation are done from node 0
	numactl --interleave=$nodeList python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-interleaveNode$nodeList-$backoff-$execTest

	# Change the kernel allocation policy: all the allocation are done from node 0
	numactl --membind=0 python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-memBindNode0-$backoff-$execTest

	exit
