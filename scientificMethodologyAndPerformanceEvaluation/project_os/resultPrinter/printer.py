import sys
import os
from data import Data
from util import getIndexInList
from util import getNbrOccurenceInList
from util import longestSubstr
from util import getIndexOfSubstringInList
from subprocess import call
from os import listdir
from os.path import isfile, join



# ---------------------------------------
# Global variables
# ---------------------------------------
defaultStatisticPath            = "../statistic/"
defaultStatisticDir             = "noName"

argumentNoPlot                  = "-noPlot"
argumentNoTest                  = "-noTest"
argumentNoTransactionalMemory   = "-noTransactionalMemory"  # allowing a group of load and store instructions to execute in an atomic way
argumentAllocatorToPrint        = "-allocator="
argumentExecutable              = "-exec="
argumentStatisticDir            = "-statDir="
argumentPlotThroughputPerCore   = "-plotThroughputPerCore="
argumentPlotErrorBar            = "-plotErrorBar"
argumentAllAttribute            = "-allAttribute"
argumentPlotAllThroughputPerCore= "-allThroughputPerCore"
argumentBackoff                 = "-backoff="
argumentNoAllocatorCompilation  = "-noAllocatorCompilation"
argumentHelp                    = "-help"

############################################################
# Version before the average values
############################################################
####### attributeArgList                = ["-nt",               "-tp",                   "-lt",                       "-fr"]
####### attributeList                   = ["Number of threads", "Throughput",            "Latency",                   "Fairness"]
####### attributeUnitList               = [None,                "# operation / second",  "# CPU cycles/ # operation", "Max # operation / Min # operation"]

############################################################
# Version After the average values
############################################################
attributeArgList                = ["-nt",               "-tp",                   "-minTp",                  "-maxTp",                  "-lt",                       "-minLt",                      "-maxLt",                      "-fr",                                "minFr",                            "maxFr"]
attributeList                   = ["Number of threads", "Throughput",            "Min throughput",          "Max throughput",          "Latency",                   "Min Latency",                 "Max Latency",                 "Fairness",                           "Min Fairness",                     "Max Fairness"]
attributeUnitList               = [None,                "# operation / second",  "# operation / second",    "# operation / second",    "# CPU cycles/ # operation", "# CPU cycles/ # operation",   "# CPU cycles/ # operation",   "Max # operation / Min # operation",  "Max # operation / Min # operation","Max # operation / Min # operation"]


defaultAttributeIndex           = 1
nbrThreadAttributeIndex         = 0
throughputAttributeIndex        = 1
throughputMinValueIndex         = 2
throughputMaxValueIndex         = 3
statDirPrintThreashold          = 10

allocatorList                   = ["Custom_Allocator", "ptmalloc",   "Hoard",    "jemalloc", "scalloc", "SuperMalloc",  "tcmalloc"]
allocatorColor                  = ["red",              "green",      "blue",     "black",    "pink",    "purple",       "cyan"]
fakeAllocatorIndex              = 0

setAllocatorExecutable          = "./setAllocator.sh"
allocatorDir                    = "../allocator"
defaultExecutableName_make      = "cc-msqueue"
knownExecutableList             = ["cc-msqueue", "mp-msqueue-1", "mp-msqueue-2", "mp-msqueue-s"]
kernelOptionList                = ["interleave", "memBind", "noOption", "noTransactionalMemory"]
statisticDirSeparator           = '+'
curvePointList                  = ['bo-', '-+', '-D', '-*', '--', '-']

defaultRunTest                  = True
defaultPlotCurv                 = True
defaultTransactionalMemory      = True
defaultCompileAllocator         = True
defaultPlotErrorBar             = False
defaultBackoffValue             = 0


# ---------------------------------------
# Local method
# ---------------------------------------
def printHelp():
    print "\n"
    print "python " + sys.argv[0] + " [option] [list of attributes to print]"

    print "Option:"
    print "\t" + argumentNoPlot
    print "\t" + argumentNoTest
    print "\t" + argumentNoTransactionalMemory
    print "\t" + argumentExecutable                 + "<name of the executable>"
    print "\t" + argumentStatisticDir               + "<list of the names of the directories containing the results separated with \'" + statisticDirSeparator + "\'>"
    print "\t" + argumentAllocatorToPrint           + "<list of the allocators separated with \',\'>"
    print "\t" + argumentPlotThroughputPerCore      + "<list of number of threads separated with \',\'>"
    print "\t" + argumentBackoff                    + "<value of the backoff (in cpu cycles)>"
    print "\t" + argumentPlotErrorBar
    print "\t" + argumentPlotAllThroughputPerCore
    print "\t" + argumentAllAttribute
    print "\t" + argumentNoAllocatorCompilation
    print "\t" + argumentHelp

    print "\nAttributes:"
    for i in xrange(len(attributeList)):
        print "\t" + attributeArgList[i] + ":\t" + attributeList[i]

    print "\nExisting stat directories:"
    dirList = [file for file in listdir(defaultStatisticPath) if not isfile(join(defaultStatisticPath, file))]
    count   = 0
    dirList.sort()
    for file in dirList:
        print "\t" + file
        count += 1
        if (count >= statDirPrintThreashold):
            break


def extractParameter():
    allocatorToPrint        = [allocatorList[i] for i in xrange(len(allocatorList))]
    attributeToPrintList    = []
    runTest                 = defaultRunTest
    plotCurv                = defaultPlotCurv
    transactionalMemory     = defaultTransactionalMemory
    executableName_make     = defaultExecutableName_make
    statisticDirList        = [defaultStatisticDir]
    backoffValue            = defaultBackoffValue
    throughputPerCoreList   = []
    recompileAllocator      = defaultCompileAllocator
    plotErrorBar            = defaultPlotErrorBar

    for argIndex in xrange(1, len(sys.argv)):
        arg = sys.argv[argIndex]
        attributeIndex  = getIndexInList(attributeArgList, arg)
        if (attributeIndex >= 0):
            attributeToPrintList.append(attributeIndex)
        elif (arg.startswith(argumentAllocatorToPrint)):
            allocatorToPrint = arg[len(argumentAllocatorToPrint) : len(arg)].split('+')
        elif (arg == argumentAllAttribute):
            attributeToPrintList = [i for i in xrange(1, len(attributeList))]
        elif (arg == argumentNoTest):
            runTest = False
        elif (arg == argumentNoPlot):
            plotCurv = False
        elif (arg == argumentNoTransactionalMemory):
            transactionalMemory = False
        elif (arg.startswith(argumentExecutable)):
            executableName_make = arg[len(argumentExecutable) : len(arg)]
        elif (arg.startswith(argumentStatisticDir)):
            statisticDirList = arg[len(argumentStatisticDir) : len(arg)].split('+')
        elif (arg.startswith(argumentPlotThroughputPerCore)):
            coreList = arg[len(argumentPlotThroughputPerCore) : len(arg)]
            throughputPerCoreList = coreList.split(',')
        elif (arg == argumentPlotAllThroughputPerCore):
            throughputPerCoreList = [argumentPlotAllThroughputPerCore]
        elif (arg.startswith(argumentBackoff)):
            backoffValue = arg[len(argumentBackoff) : len(arg)]
        elif (arg == argumentNoAllocatorCompilation):
            recompileAllocator = False
        elif (arg == argumentPlotErrorBar):
            plotErrorBar = True
        elif (arg == argumentHelp):
            printHelp()
            exit()
        else:
            print "Unknown argument: " + arg
            printHelp()
            exit()

    if (len(attributeToPrintList) == 0):
        attributeToPrintList.append(defaultAttributeIndex)

    return (allocatorToPrint, attributeToPrintList, runTest, plotCurv, transactionalMemory, executableName_make, statisticDirList, throughputPerCoreList, backoffValue, recompileAllocator, plotErrorBar)


def parseExecutableName(statisticDir):
    for executable in knownExecutableList:
        if (statisticDir.find(executable) >= 0):
            return executable
    return "Unknown"


def runUnitTest(allocatorName, executableName_make, statisticPath, transactionalMemory, backoffValue, recompileAllocator):
    # Set the allocator as a default library and compile test using 
    execArguments = [setAllocatorExecutable, allocatorName, executableName_make, statisticPath, str(transactionalMemory), str(backoffValue), str(recompileAllocator)]
    call(execArguments, shell=False)


def plotAttributeCurve(dataList, attributeToPrint, statisticDirList, plotErrorBar):
    import matplotlib.pyplot as plt
    plt.figure()
    for attribute in attributeToPrint:
        plotedAllocator = []
        for data in dataList:
            if (data == None):
                continue
            nbrThreadList   = data.getAttributeValueList_fromIndex(nbrThreadAttributeIndex)
            attributeValue  = data.getAttributeValueList_fromIndex(attribute)
            allocatorIndex  = getIndexInList(allocatorList, data.getAllocator())
            curvePoint      = curvePointList[getNbrOccurenceInList(plotedAllocator, data.getAllocator())]
            alloc           = data.getAllocator()

#            errorMin        = [int(i) for i in data.getAttributeValueList_fromIndex(throughputMinValueIndex)]
#            errorMax        = [int(i) for i in data.getAttributeValueList_fromIndex(throughputMaxValueIndex)]
            if (len(dataList) > len(allocatorList)):
                currentLabel = data.getExecutable() + "/"  + data.getKernelOption() + "/" + alloc
            else:
                currentLabel = alloc
            currentLabel = currentLabel + "-" + str(data.getBackoff())
            plt.plot(nbrThreadList, attributeValue, curvePoint, label=currentLabel, color=allocatorColor[allocatorIndex])
#            if (plotErrorBar):
#                plt.errorbar(nbrThreadList, attributeValue, yerr=errorMin, fmt='-o')
            plotedAllocator.append(data.getAllocator())

        plt.grid()
        plt.xlabel(attributeList[nbrThreadAttributeIndex])
        if (attributeUnitList[attribute] != None):
            plt.ylabel(attributeList[attribute] + '(' + attributeUnitList[attribute] + ')')
        else:
            plt.ylabel(attributeList[attribute])
        if (len(statisticDirList) == 1):
            titleTab = statisticDirList[0].split('/')
            titleTab = titleTab[len(titleTab)-1 : len(titleTab)]
            title    = '(' + titleTab[0] + ')'
        else:
            title = longestSubstr(statisticDirList)
            if (title != ''):
                title = '(' + title + ')'

        plt.title('Performance comparison' + title)
        plt.legend(loc=2)  # Put the legend on the left
        plt.show()

def plotThroughputPerCore(dataList, nbrThread):
    import matplotlib.pyplot as plt
    import numpy as np
    plt.figure()

    width       = 0.12
    ind         = None
    initialized = False

    for allocatorIndex in xrange(len(allocatorList)):
        data = dataList[allocatorIndex]
        if (data == None):
            continue
        if (initialized == False):
            ind          = np.arange(data.getNbrCore())
            initialized  = True
        throughputPerCore= data.getThroughputPerCore(nbrThread, nbrThreadAttributeIndex)
#####TODO
        plt.bar(ind+(allocatorIndex*width), throughputPerCore, width, label=data.getAllocator(), color=allocatorColor[allocatorIndex])

    plt.ylabel(attributeList[throughputAttributeIndex] + '(' + attributeUnitList[throughputAttributeIndex] + ')')
    plt.xlabel('Core')
    plt.xticks(ind + width, [str(i) for i in xrange(data.getNbrCore())])
    plt.title('Throughput per core (' + statisticDir + ')')
    plt.legend()
    plt.tight_layout()
    plt.show()


# ---------------------------------------
# Main method
# ---------------------------------------
if __name__ == "__main__":
    (allocatorToPrint, attributeToPrint, runTest, plotCurv, transactionalMemory, executableName_make, statisticDirList, throughputPerCoreList, backoffValue, recompileAllocator, plotErrorBar) = extractParameter()
    dataList = []

    for statisticDir in statisticDirList:
        statisticPath   = defaultStatisticPath + statisticDir
        kernelOptionInd = getIndexOfSubstringInList(kernelOptionList, statisticDir)
        if (kernelOptionInd < 0):
            kernelOpt = ''
        else:
            kernelOpt = kernelOptionList[kernelOptionInd]
        for allocatorName in allocatorList:
            ind = getIndexInList(allocatorToPrint, allocatorName)
            if (ind < 0):
                data = None
            else:
                if (runTest):
                    runUnitTest(allocatorName, executableName_make, statisticPath, transactionalMemory, backoffValue, recompileAllocator)
                fileName= str(statisticPath + "/" + allocatorName)
                if (os.path.isfile(fileName)):
                    fic         = open(fileName)
                    executable  = parseExecutableName(statisticDir)
                    data        = Data(executable, fic, attributeList, kernelOption=kernelOpt)
                    fic.close()
                else:
                    data = None
            dataList.append(data)

    if (plotCurv):
        plotAttributeCurve(dataList, attributeToPrint, statisticDirList, plotErrorBar)
        if (throughputPerCoreList == [argumentPlotAllThroughputPerCore]):
            throughputPerCoreList = data.getAttributeValueList_fromIndex(nbrThreadAttributeIndex)
        for nbrThread in throughputPerCoreList:
            plotThroughputPerCore(dataList, nbrThread);

