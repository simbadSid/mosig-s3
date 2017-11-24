from util import nextDataLine
from util import nextMeaningfullLine





class Data:
# -----------------------------
# Attributes
# -----------------------------
    """
        executable
        kernelOption
        allocator           = ""    # Name of the allocator
        nbrNode             = int   # Number of numa node on the host machine
        nbrCore             = int   # Total number of cores on the host machine
        backOff             = int
        nbrEntry            = int   # Number of lines
        attributeList       = []    # List of the name of the attributes
        dataAttributeMatrix = [[]]  # dataAttributeMatrix[attribute][line]
        coreThroughput      = [[]]  # coreThroughput[line][core]
    """

    """
        Row Data file:
            - Allocator name
            - Number of NUMA nodes
            - Total number of CPU
            - Backoff value (used in the optimisation 2 in the paper of Thomas Ropars)
            - Number of lines (experiences)
    """
# -----------------------------
# Builder
# -----------------------------
    def __init__(self, executable, file, attributeList, kernelOption=''):
        self.executable             = executable
        nbrAttribute                = len(attributeList)
        self.kernelOption           = kernelOption
        self.allocator              = nextMeaningfullLine(file)
        self.nbrNode                = int(nextMeaningfullLine(file))
        self.nbrCore                = int(nextMeaningfullLine(file))
        self.backoff                = int(nextMeaningfullLine(file))
        self.attributeList          = attributeList
        self.nbrEntry               = int(nextMeaningfullLine(file))
        self.dataAttributeMatrix    = [[0 for j in xrange(self.nbrEntry)] for i in xrange(nbrAttribute)]
        self.coreThroughput         = [[0 for j in xrange(self.nbrCore)] for i in xrange(self.nbrEntry)]

        for entry in xrange(self.nbrEntry):                                             # For each line of the file
            dataLine = nextDataLine(file)
            for attribute in xrange(nbrAttribute):                                      #    For the 1st part of the line: list of attributes
                self.dataAttributeMatrix[attribute][entry] = dataLine[attribute]
            for core in xrange(self.nbrCore):                                           #    For the 2nd part of the line: core throughput
                self.coreThroughput[entry][core] = int(dataLine[nbrAttribute+core])

# -----------------------------
# Getter
# -----------------------------
    def getExecutable(self):
        return self.executable


    def getAllocator(self):
        return self.allocator


    def getKernelOption(self):
        return self.kernelOption


    def getNbrNode(self):
        return self.nbrNode


    def getNbrCore(self):
        return self.nbrCore


    def getBackoff(self):
        return self.backoff


    def getNbrAttribute(self):
        return len(self.attributeList)


    def getAttributeValueList_fromIndex(self, attributeIndex):
        return self.dataAttributeMatrix[attributeIndex]


    def getAttributeValueList_fromName(self, attributeName):
        for index in xrange(self.getNbrAttribute()):
            if (attributeName == self.attributeList[index]):
                return self.dataAttributeMatrix[index]


    def getThroughputPerCore(self, nbrThread, nbrThreadAttributeIndex):
        for line in xrange(self.nbrEntry):
            if (self.dataAttributeMatrix[nbrThreadAttributeIndex][line] == nbrThread):
                return self.coreThroughput[line]
        raise Exception("Can't find the experience with " + nbrThread + " threads")
