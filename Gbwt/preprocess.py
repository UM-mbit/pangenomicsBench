"""
This is a quick script for parsing a gbz and generating a bunch of path queries.
Usage
  python preprocess.py <filename.gbz>
It outputs a file where each line is a query path in the graph sampled from the
haplotypes. Additional parameters can be passed to control the number of outputs

Outputs are hardcoded to queries.txt and g.gbwt in an inputs folder, which is
what the kernel expects
"""
import random
import os
import argparse

import pandas as pd


def initArgs():
    '''
    initialize argparser, add arguments and returns args
    '''
    # Create the argument parser
    parser = argparse.ArgumentParser()

    # Define the arguments
    parser.add_argument('--lowRange', type=int, default=1, 
            help='The smallest query we want to generate (default: 1)')
    parser.add_argument('--highRange', type=int, default=10, 
            help='The largest query we want to generate (default: 10)')
    parser.add_argument('--inputFile', type=str, required=True, 
            help='Input filepath to a gbz (required)')
    parser.add_argument('--queriesPerHaplotype', type=int, default=500, 
            help='Queries per haplotype (default: 500)')
    parser.add_argument('--randomSeed', type=int, default=42, 
            help='Random Seed (default:42)')

    # Parse the arguments
    args = parser.parse_args()
    return args

def sampleHaplotype(hapl, lowRange, highRange, nQueries):
    '''
    Extracts nQueries sample queries from hapl with lengths uniformly
    distributed between lowRange and highRange.
    @param list<int> a haplotype path through the graph with node ids
    @param int lowRange the shortest query we want
    @param int highRange the largest query we want to generate
    @param int nQueries how many queries to extract
    @returns list<list<int>> a bunch of samples haplotypes
    '''
    queries = []
    for i in range(nQueries):
        queryLen = random.randint(lowRange, highRange)
        startRange = len(hapl) - queryLen
        start = random.randint(0, startRange)
        query = []
        for i in range(start, start+queryLen):
            query.append(hapl[i])
        queries.append(query)
    return queries

def initDir(name):
    """
    creates an output dir with name, and if it exists adds -1...
    """
    newName = name
    built = False
    i = 1
    while (not built):
        if not os.path.exists(newName):
            os.mkdir(newName)
            built = True
        else:
            newName = "{}-{}".format(name, i)
        i += 1
    return newName


def main():
    args = initArgs();
    outDir = initDir("Inputs")
    random.seed(args.randomSeed)

    #generate gbwt output
    os.system("vg gbwt -Z {} -o {}.gbwt".format(
        args.inputFile, os.path.join(outDir, "g")))
    #generate temporary gaf of the paths
    os.system("vg paths -x {} -A -H> {}".format(
        args.inputFile, os.path.join(outDir, ".tmpExtractedPaths.gaf")))

    #parse the haplotypes from the file
    rawDf = pd.read_csv(os.path.join(outDir, ".tmpExtractedPaths.gaf"), 
            delimiter="\t", header=None)
    #some have backwards paths. We don't consider these. Just forward paths
    df = rawDf.loc[~rawDf[5].apply(lambda x: '<' in x)] 
    haplotypes = df[5].apply(lambda x: [int(s) for s in x[1:].split('>')])
    
    #sample the haplotypes
    allQueries = []
    for hapl in haplotypes:
        haplQueries = sampleHaplotype(hapl, args.lowRange, 
                                      args.highRange, args.queriesPerHaplotype)
        allQueries.append(haplQueries)

    #dump the queries to a file
    with open(os.path.join(outDir, "queries.txt"), "w") as wfile:
        for haplGroup in allQueries:
            for query in haplGroup:
                queryStr = ""
                for nodeId in query:
                    queryStr += str(nodeId)
                    queryStr += ">"
                wfile.write(queryStr[:-1])
                wfile.write("\n")

    #cleanup
    os.system("rm {}".format(os.path.join(outDir, ".tmpExtractedPaths.gaf")))

if __name__ == '__main__':
    main()

