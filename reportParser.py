import re

def getCpi(s):
    return float(re.search(r"CPI Rate: (\d+\.\d+)", s).groups()[0])

def getRetiring(s):
    return float(re.search(r"Retiring: (\d+\.\d+)%", s).groups()[0])

def getFrontEndBound(s):
    return float(re.search(r"Front-End Bound: (\d+\.\d+)%", s).groups()[0])

def getCoreBound(s):
    return float(re.search(r"Core Bound: (\d+\.\d+)%", s).groups()[0])

def getBadSpeculationBound(s):
    return float(re.search(r"Bad Speculation: (\d+\.\d+)%", s).groups()[0])

def getMemBound(s):
    return float(re.search(r"Memory Bound: (\d+\.\d+)%", s).groups()[0])

def parseUarchReport(s):
    stats = "" 
    stats += "Cpi " + str(getCpi(s)) + "\n"
    stats += "Retiring " + str(getRetiring(s)) + "\n"
    stats += "FrontEndBound " + str(getFrontEndBound(s)) + "\n"
    stats += "CoreBound " + str(getCoreBound(s)) + "\n"
    stats += "BadSpeculationBound " + str(getBadSpeculationBound(s)) + "\n"
    stats += "MemBound " + str(getMemBound(s)) + "\n"
    return stats

def loadFile(filename):
    '''
    read file to string. Saves typing
    '''
    data = ""
    with open(filename) as iFile:
        data = iFile.read();
    return data


def main():
    print(parseUarchReport(loadFile("uarchSum.txt")))

if __name__ == "__main__":
    main()
