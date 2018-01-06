import datetime
import sys
from subprocess import call
from util import isEndOfFile
from util import nextMeaningfullLine
from util import nextDataLine






# ---------------------------------------
# Global variables
# ---------------------------------------
rootDir                         = "grid5000"
cityFileName                    = rootDir + "/city"
setRemeoteAllocatorExecutable   = "./setRemoteAllocator_master.sh"
setLocalAllocatorExecutable     = "./setLocalAllocator.sh"
getRemoteResultExecutable       = "./getRemoteResult.sh"

argumentCurrentCity             = "-currentCity="
argumentCurrentServer           = "-currentServer="
argumentResultDir               = "-resultDir="
argumentExperienceName          = "-experience="
argumentImportAllRes            = "-importRes"
argumentGrid5000Remote          = "-grid5000"
argumentExecTest                = "-execTest="
argumentBackoff                 = "-backoff="
argumentHelp                    = "-help"

defaultCurrentCity              = "somewhereOverTheRainbow"
defaultCurrentServer            = "NSA-KHORDA"
defaultResultDir                = "../statistic/Remote"
defaultExperienceName           = "Memalloc_Remote"
defaultExecTest                 = "cc-msqueue"
defaultBackoff                  = "0"

maxNbrRemoteHost                = 20


# ---------------------------------------
# Local method
# ---------------------------------------
def printHelp():
    print "\n"
    print "python " + sys.argv[0] + " [option]"

    print "Option:"
    print "\t" + argumentResultDir      + "<name of the result directory>"
    print "\t" + argumentExecTest       + "<name of the executable>"
    print "\t" + argumentBackoff        + "<value of the backoff (integer >= 0)>"
    print "\t" + argumentImportAllRes   + " : Import previously computed results (no test is run)"
    print "\t" + argumentGrid5000Remote + "  : Start the executions on the remote servers (need to be launched on the Grid500 network )"
    print "\t" + argumentCurrentServer  + "<name of the current server> (optional)"
    print "\t" + argumentCurrentCity    + "<name of the current city> (optional)"
    print "\t" + argumentExperienceName + "<name of the experience> (optional)"
    print "\t" + argumentHelp


def extractParameter():
    currentServer   = defaultCurrentServer
    currentCity     = defaultCurrentCity
    isGrid5000Remote= False
    resultDir       = defaultResultDir + "-" + str(datetime.datetime.now()).replace(" ", "_")
    importAllRes    = False
    experienceName  = defaultExperienceName
    execTest        = defaultExecTest
    backoff         = defaultBackoff

    for argIndex in xrange(1, len(sys.argv)):
        arg = sys.argv[argIndex]
        if (arg.startswith(argumentCurrentServer)):
            currentServer = arg[len(argumentCurrentServer) : len(arg)]
        elif (arg.startswith(argumentCurrentCity)):
            currentCity = arg[len(argumentCurrentCity) : len(arg)]
        elif (arg.startswith(argumentGrid5000Remote)):
            isGrid5000Remote = True
        elif (arg.startswith(argumentResultDir)):
            resultDir = arg[len(argumentResultDir) : len(arg)]
        elif (arg == argumentImportAllRes):
            importAllRes = True
        elif (arg.startswith(argumentExperienceName)):
            experienceName = arg[len(argumentExperienceName) : len(arg)]
        elif (arg.startswith(argumentExecTest)):
            execTest = arg[len(argumentExecTest) : len(arg)]
        elif (arg.startswith(argumentBackoff)):
            backoff = arg[len(argumentBackoff) : len(arg)]
        elif (arg == argumentHelp):
            printHelp()
            exit()
        else:
            print "Unknown argument: " + arg
            printHelp()
            exit()

    return (currentCity, currentServer, isGrid5000Remote, importAllRes, resultDir, experienceName, execTest, backoff)


def runTest(cityName, serverName, serverSpec, sendProjectToHost, experienceName, execTest, backoff, isFirstCall, isGrid5000Remote):
    execArguments = [cityName, serverName, serverSpec, sendProjectToHost, experienceName, execTest, backoff, isFirstCall]
    execArguments = map(str, execArguments)
    if (isGrid5000Remote):
        execArguments.insert(0, setRemeoteAllocatorExecutable)
    else:
        execArguments.insert(0, setLocalAllocatorExecutable)
    call(execArguments, shell=False)


def importRemoteTest(cityName, serverSpec, resultDir):
    execArguments = [getRemoteResultExecutable, cityName, serverSpec, resultDir]
    call(execArguments, shell=False)


# ---------------------------------------
# Main method
# ---------------------------------------
if __name__ == "__main__":
    (currentCity, currentServer, isGrid5000Remote, importAllRes, resultDir, experienceName, execTest, backoff) = extractParameter()
    cityFile        = open(cityFileName)
    remoteHostCount = 0
    isFirstCall     = True

    if (not isGrid5000Remote):
        runTest(currentCity, currentServer, currentServer, False, experienceName, execTest, backoff, False, isGrid5000Remote)
        
    else:
        while ((not isEndOfFile(cityFile)) and (remoteHostCount < maxNbrRemoteHost)):
            cityName    = nextMeaningfullLine(cityFile)
            serverFile  = open(rootDir + "/" + cityName)
            sendProjectToHost = not (currentCity == cityName)
            while (not isEndOfFile(serverFile)):
                (serverName, serverSpec) = nextDataLine(serverFile)
                if (currentServer == serverName):
                    continue
#                print "-----------------------------------------"
#                print "\t- City       = " + cityName
#                print "\t- ServerName = " + serverName
#                print "\t- ServerSpec = " + serverSpec
#                print "-----------------------------------------"
                if (not importAllRes):
                    runTest(cityName, serverName, serverSpec, sendProjectToHost, experienceName, execTest, backoff, isFirstCall, isGrid5000Remote)
                else:
                    importRemoteTest(cityName, serverSpec, resultDir)
                sendProjectToHost   = False
                isFirstCall         = False
            serverFile.close()
            remoteHostCount += 1
    
        cityFile.close()