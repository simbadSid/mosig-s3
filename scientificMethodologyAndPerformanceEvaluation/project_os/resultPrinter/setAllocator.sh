#!/bin/bash


#---------------------------------------------------------
# Parameters of the script
#---------------------------------------------------------
	allocatorName=$1
	executableName_make=$2
	statisticDir=$3
	useTransactionalMemory=$4
	backoffValue=$5
	compilAllocator=$6

#---------------------------------------------------------
# Global (final) parameters
#---------------------------------------------------------
	makeEnvParam="allocatorName"
	backoffParam="backoffValue"
	executableName="$executableName_make.run"
	testerDir="../tester"
	allocatorDir="../allocator"
	defaultSudoCommand="sudo-g5k"



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
echo "--------------------------------"
echo "Set default allocator: $allocatorName"
echo "--------------------------------"
export $makeEnvParam=$allocatorName
export $backoffParam=$backoffValue




initialPath=`pwd`
cd $allocatorDir
mkdir $statisticDir
allocatorLibPath=""



if [ "$allocatorName" == "Custom_Allocator" ]; then
	allocatorLibPath=""





elif [ "$allocatorName" == "ptmalloc" ]; then
	allocatorLibPath=""






elif [ "$allocatorName" == "Hoard" ]; then
	cd $allocatorName
	allocatorLibPath=`pwd`"/src/libhoard.so"
	if [ "$compilAllocator" == "True" ]; then
		make -C "src" clean
		make -C "src"
	fi





elif [ "$allocatorName" == "scalloc" ]; then
	cd $allocatorName
	allocatorLibPath=`pwd`"/out/Release/lib.target/libscalloc.so"
	if [ "$compilAllocator" == "True" ]; then
		tools/make_deps.sh
		build/gyp/gyp --depth=. scalloc.gyp
		BUILDTYPE=Debug make
		BUILDTYPE=Release make
	fi
	# Change the kernel allocation policy:
	#	Allow over commit: alow to allocate memory even if not physically existing
	if hash $defaultSudoCommand 2>/dev/null; then
		sudoCommand="$defaultSudoCommand"
	else
		sudoCommand="sudo"
#		sudoCommand=""
	fi
	$sudoCommand /sbin/sysctl vm.overcommit_memory=1





elif [ "$allocatorName" == "jemalloc" ]; then
	cd $allocatorName
	allocatorLibPath=`pwd`"/lib/libjemalloc.so"
	if [ "$compilAllocator" == "True" ]; then
		./autogen.sh
		make build_lib
	fi




elif [ "$1" == "SuperMalloc" ]; then
	cd $allocatorName/release
	if [ $useTransactionalMemory == "True" ]; then
		allocatorLibPath=`pwd`"/lib/libsupermalloc.so"
	else
		allocatorLibPath=`pwd`"/lib/libsupermalloc_pthread.so"
	fi
	if [ "$compilAllocator" == "True" ]; then
		make
		make check
	fi




elif [ "$1" == "tcmalloc" ]; then
	cd $allocatorName
	allocatorLibPath=`pwd`"/.libs/libtcmalloc.so"
	if [ "$compilAllocator" == "True" ]; then
		./autogen.sh
		./configure
		make
	fi






else
	echo "********************************"
	echo "Unknown allocator: $allocatorName"
	echo "********************************"
	exit
fi


# Compile the test program using the given allocator
export LD_PRELOAD=$allocatorLibPath

cd $initialPath
echo "-------------------------------- Begin executable compilation"
make -C $testerDir clean
make -C $testerDir $executableName_make
echo "-------------------------------- End executable compilation"

echo
echo
echo "-------------------------------- Begin test run"
echo "allocator library = $allocatorLibPath"
$testerDir/$executableName -allocName $allocatorName -statFile $statisticDir/$allocatorName
echo "-------------------------------- End test run"
echo
echo

export LD_PRELOAD=
