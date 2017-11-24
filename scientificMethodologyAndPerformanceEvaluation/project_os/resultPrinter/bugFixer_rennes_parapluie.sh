#!/bin/bash




#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
	cityName=rennes
	serverName=parapluie
	serverSpec=24-rennes-parapluie-AMD_Opteron_6164_HE
	sendProjectToHost=False
	experienceName=DEBUG
	execTest=$1
	backoff=$2


#---------------------------------------------------------
# Main script
#---------------------------------------------------------
	# Default allocation policy: First touch: Allocations done on the node where it is ordered
	python printer.py -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-noOption-$backoff-$execTest

	# Forbid the transactional memory: usage of atomic memory instruction for synchronization
#	python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -noTransactionalMemory -statDir=$serverSpec-noTransactionalMemory-$backoff-$execTest

	# Change the kernel allocation policy: all the allocation are done from node 0
#	numactl --interleave=$nodeList python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-interleaveNode$nodeList-$backoff-$execTest

	# Change the kernel allocation policy: all the allocation are done from node 0
	numactl --membind=0 python printer.py -noAllocatorCompilation -exec=$execTest -noPlot -backoff=$backoff -statDir=$serverSpec-memBindNode0-$backoff-$execTest

	exit
