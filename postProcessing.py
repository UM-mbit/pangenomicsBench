import pandas as pd
import matplotlib.pyplot as plt
import numpy as np



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
        if ("HIT" in name):
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

def plotBars(labels, *dicts):
    df = pd.DataFrame(dicts, index=labels)
    df.plot(kind="bar", stacked=True)

def plotFiveBars(func, file, plotname):
    '''
    quick and dirty test
    '''
    plotBars(["dummy{}".format(i) for i in range(5)],
            *[func("DummyOut/Results/{}".format(file)) for i in
                range(5)])
    plt.savefig(plotname)

if __name__ == "__main__":
    print(loadCacheStats("DummyOut/Results/cacheStats.txt"))
    print(loadUarchStats("DummyOut/Results/uarchStats.txt"))
    print(loadInstrCounts("DummyOut/Results/instrCounts.txt"))
    plotFiveBars(loadCacheStats, "cacheStats.txt", "cache.png")
    plotFiveBars(loadUarchStats, "uarchStats.txt", "uarch.png")
    plotFiveBars(loadInstrCounts, "instrCounts.txt", "instrCounts.png")
