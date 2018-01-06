import os





# ---------------------------------------
# Global variables
# ---------------------------------------
DEFAULT_COMMENT         = "#"
DEFAULT_DATA_SEPARATOR  = " "




# ---------------------------------------
# Local method
# ---------------------------------------
def isEndOfFile(fic):
    return (fic.tell() == os.fstat(fic.fileno()).st_size)


def nextMeaningfullLine(fic, commentString=DEFAULT_COMMENT):
    while (not isEndOfFile(fic)):
        res = fic.readline().strip()
        if (res.startswith(commentString)):
            continue
        elif (res == "\n" or res == ""):
            continue
        else:
            return res
    raise Exception("No useful string found in the file " + fic.name)

def nextDataLine(fic):
    return nextMeaningfullLine(fic).split(DEFAULT_DATA_SEPARATOR)


def getIndexInList(list, item):
    for index in xrange(len(list)):
        if (item == list[index]):
            return index
    return -1


def getIndexOfSubstringInList(list, string):
    for i in xrange(len(list)):
        str = list[i]
        if (str in string):
            return i
        if (string in str):
            return i
    return -1


def getNbrOccurenceInList(list, item):
    res = 0
    for elem in list:
        if (item == elem):
            res = res + 1
    return res


def getColorByName(colorName):
    import matplotlib
    for name, hex in matplotlib.colors.cnames.iteritems():
        if (name == colorName):
            return hex
    raise Exception ("Unknown color: " + colorName)


def isSubstr(str, stringTab):
    if len(stringTab) < 1 and len(str) < 1:
        return False
    for i in range(len(stringTab)):
        if str not in stringTab[i]:
            return False
    return True


def longestSubstr(stringTab):
    substr = ''
    if len(stringTab) > 1 and len(stringTab[0]) > 0:
        for i in range(len(stringTab[0])):
            for j in range(len(stringTab[0])-i+1):
                if j > len(substr) and isSubstr(stringTab[0][i:i+j], stringTab):
                    substr = stringTab[0][i:i+j]
    return substr



