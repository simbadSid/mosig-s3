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
	isFirstCall=$8


#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
	execSlave=setRemoteAllocator_slave.sh
	projectSrc=../../internship_mem_alloc_project
#	foreignHost="rsidlakhdar@access.grid5000.fr"


#---------------------------------------------------------
# Main script
#---------------------------------------------------------
	if [ $sendProjectToHost == "True" ]; then
		if [ $isFirstCall == "True" ]; then
			tar cvf $projectSrc.tar $projectSrc
		fi
		scp -r $projectSrc.tar $cityName:
######		rm -f $projectSrc.tar
	fi

	ssh $cityName 'bash -s' < $execSlave $*

	# Do not get the result immediately:
	# Not synchronized with the remote server
