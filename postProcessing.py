import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import os

#prevent labels from being cut off
plt.rcParams['figure.constrained_layout.use'] = True
plt.rcParams['font.size'] = 14  # Set to desired size



def loadFile(filepath):
    '''
    saves typing. load file to a list of lines
    '''
    with open(filepath, 'r') as iFile:
        data = iFile.readlines()
    return data

def loadCacheStats(filepath):
    '''
    load in cache statistics percentages
    '''
    d = {}
    for line in loadFile(filepath):
        name, val = line.split(" ")
        val=float(val)
        if ("Miss" in name):
            d[name] = val*100
    return d

def loadUarchStats(filepath):
    '''
    load in uarch stat percentages
    '''
    d = {}
    for line in loadFile(filepath):
        name, val = line.split(" ")
        val=float(val)
        if (name != "Cpi"):
            d[name] = val
    return d

def loadInstrCounts(filepath):
    '''
    load in instrCounts
    '''
    d = {}
    for line in loadFile(filepath):
        name, val = line.split(" ")
        val=float(val)
        if (name != "Count"):
            d[name] = val
    return d

def plotBars(labels, dicts):
    df = pd.DataFrame(dicts, index=labels)
    df.plot(kind="bar", stacked=True)

#def plotFiveBars(func, file, plotname):
#    '''
#    quick and dirty test
#    '''
#    plotBars(["dummy{}".format(i) for i in range(5)],
#            [func("DummyOut/Results/{}".format(file)) for i in
#                range(5)])
#    plt.savefig(plotname)

def plotForDir(parsingFunc, filename, plotname):
    statBarNames = [d for d in os.listdir("AllRunsOut") if 
            os.path.isfile("AllRunsOut/{}/Results/{}".format(d, filename))]
    statBarFiles = ["AllRunsOut/{}/Results/{}".format(d,filename) for d in
            statBarNames]
    statDicts = [parsingFunc(file) for file in statBarFiles]
    plotBars(statBarNames, statDicts)
    plt.savefig(plotname)

def printDirContents(filename):
    statBarNames = [d for d in os.listdir("AllRunsOut") if 
            os.path.isfile("AllRunsOut/{}/Results/{}".format(d, filename))]
    statBarFiles = ["AllRunsOut/{}/Results/{}".format(d,filename) for d in
            statBarNames]
    for name, file in zip(statBarNames, statBarFiles):
        with open(file, 'r') as f:
            print(name)
            print(f.read())


if __name__ == "__main__":
    plotForDir(loadCacheStats, "cacheStats.txt", "cache.png")
    plotForDir(loadUarchStats, "uarchStats.txt", "uarch.png")
    plotForDir(loadInstrCounts, "instrCounts.txt", "instrCounts.png")
    plotForDir(loadUarchStats, "uarchStats.txt", "uarch.png")
    printDirContents("vanillaRunTimes.txt")
    printDirContents("threadScaling.txt")
    #cacheBarNames = [d for d in os.listdir("AllRunsOut") if 
    #        os.path.isfile("AllRunsOut/{}/Results/cacheStats.txt".format(d))]
    #cacheBarFiles = ["AllRunsOut/{}/Results/cacheStats.txt".format(d) for d in
    #        cacheBarNames]
    #cacheDicts = [loadCacheStats(file) for file in cacheBarFiles]
    #plotBars(cacheBarNames, cacheDicts)
    #plt.show()
    #print(cacheBarNames, cacheBarFiles)
    #print(loadCacheStats("DummyOut/Results/cacheStats.txt"))
    #print(loadUarchStats("DummyOut/Results/uarchStats.txt"))
    #print(loadInstrCounts("DummyOut/Results/instrCounts.txt"))
    #plotFiveBars(loadCacheStats, "cacheStats.txt", "cache.png")
    #plotFiveBars(loadUarchStats, "uarchStats.txt", "uarch.png")
