#!/bin/bash




#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
	cityName=rennes_w
	serverName=paranoia_w
	serverSpec=20-rennes-paranoia-Intel_Xeon_E5_2620_w
	sendProjectToHost=False
	experienceName=DEBUG

	declare -a execTestMPList=("mp-msqueue-1" "mp-msqueue-2" "mp-msqueue-s")
	declare -a backoffList=("0" "2500" "5000" "7500" "10000" "15000")


#---------------------------------------------------------
# Main script
#---------------------------------------------------------
execTest=cc-msqueue
backoff=0

	# Forbid the transactional memory: usage of atomic memory instruction for synchronization
	python printer.py -exec=$execTest -noPlot -backoff=$backoff -noTransactionalMemory -statDir=$serverSpec-noTransactionalMemory-$backoff-$execTest

	# Default allocation policy: First touch: Allocations done on the node where it is ordered
	python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-noOption-$backoff-$execTest

	# Change the kernel allocation policy: all the allocation are done from node 0
	numactl --interleave=$nodeList python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-interleaveNode$nodeList-$backoff-$execTest

	# Change the kernel allocation policy: all the allocation are done from node 0
	numactl --membind=0 python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-memBindNode0-$backoff-$execTest






	for execTest in "${execTestMPList[@]}"
	do
		for backoff in "${backoffList[@]}"
		do
			# Forbid the transactional memory: usage of atomic memory instruction for synchronization
			python printer.py -exec=$execTest -noPlot -backoff=$backoff -noTransactionalMemory -statDir=$serverSpec-noTransactionalMemory-$backoff-$execTest

			# Default allocation policy: First touch: Allocations done on the node where it is ordered
			python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-noOption-$backoff-$execTest

			# Change the kernel allocation policy: all the allocation are done from node 0
			numactl --interleave=$nodeList python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-interleaveNode$nodeList-$backoff-$execTest

			# Change the kernel allocation policy: all the allocation are done from node 0
			numactl --membind=0 python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-memBindNode0-$backoff-$execTest

		done
	done

	exit
