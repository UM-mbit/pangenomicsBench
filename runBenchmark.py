import os
import shutil
import re
import pandas as pd

HOME="." # this is the root directory of the pangenomicsBench


class UarchReportParser:
    """
    Class for parsing a uarch report. It extracts cpi, retiring, frontent bound
    core bound, bad spec, and mem bound.
    It's essentially a bunch of grep functions and one function for getting
    everything and pretty formating
    """

    def getCpi(self, s):
        return float(re.search(r"CPI Rate: (\d+\.\d+)", s).groups()[0])

    def getRetiring(self, s):
        return float(re.search(r"Retiring: (\d+\.\d+)%", s).groups()[0])

    def getFrontEndBound(self, s):
        return float(re.search(r"Front-End Bound: (\d+\.\d+)%", s).groups()[0])

    def getCoreBound(self, s):
        return float(re.search(r"Core Bound: (\d+\.\d+)%", s).groups()[0])

    def getBadSpeculationBound(self, s):
        return float(re.search(r"Bad Speculation: (\d+\.\d+)%", s).groups()[0])

    def getMemBound(self, s):
        return float(re.search(r"Memory Bound: (\d+\.\d+)%", s).groups()[0])

    def parseUarchReport(self, filename):
        s = self.loadFile(filename)
        stats = "" 
        stats += "Cpi " + str(self.getCpi(s)) + "\n"
        stats += "Retiring " + str(self.getRetiring(s)) + "\n"
        stats += "FrontEndBound " + str(self.getFrontEndBound(s)) + "\n"
        stats += "CoreBound " + str(self.getCoreBound(s)) + "\n"
        stats += "BadSpeculationBound " + str(self.getBadSpeculationBound(s)) + "\n"
        stats += "MemBound " + str(self.getMemBound(s)) + "\n"
        return stats

    def loadFile(self, filename):
        '''
        read file to string. Saves typing
        '''
        data = ""
        with open(filename, "r") as iFile:
            data = iFile.read();
        return data

'''
This is quite brittle. If you're reading this, you should probably just start
from scratch.
It produces a list of tuples with name and the value of that hw counter
'''

class CacheReportParser:
    '''
    This one is spaghetti code, sorry. It should work though. 
    '''
    def generateTupleList(self, dataFile):
        data = ""

        with open(dataFile, 'r') as rFile:
            data = rFile.read()

        PREFIX = r"MEM_LOAD_RETIRED\."
        METRICS = ["L1_HIT", "L1_MISS", "L2_HIT", "L2_MISS", "L3_HIT", "L3_MISS",
        "FB_HIT"]

        results = []
        for reQuery in [r"{}({}) *(\d*)".format(PREFIX, metric) for metric in METRICS]:
            results.append(re.search(reQuery, data).groups())
        results.append(("DRAM_HIT", re.search(
            r'MEM_LOAD_L3_MISS_RETIRED\.LOCAL_DRAM *(\d*)', data).groups()[0]))
        results.append(("ALL_LOADS", re.search(
            r'MEM_INST_RETIRED\.ALL_LOADS *(\d*)', data).groups()[0]))
        return pd.Series({key:val for key, val in results}, dtype=int)

    def formatSeriesOut(self, data):
        '''
        formats the tuple list output of generateTupleList. Might be destructive
        of the series
        '''
        outStr = ""
        total = data["ALL_LOADS"]
        data /= total
        for key, val in data.items():
            outStr += "{} {}\n".format(key, val)
        return outStr
    
    def parseCacheReport(self, dataFile):
        '''
        Parses the cache data and returns a string nicely formated of useful
        information
        '''
        print(self.generateTupleList(dataFile))
        return self.formatSeriesOut(self.generateTupleList(dataFile))

class Runner:

    def __init__(
        self, 
        outDir,
        vtune="/opt/intel/oneapi/vtune/2024.1/bin64/vtune",
        pinLocalPath="ProfilingTools/IntelPin"
    ):
        """
        @params string: outDir the ouput directory to store all the files
        @params string: app the application to run with all args
        @params string: vtune the path of the vtune application to be used
        @params string: pinLocalPath local path from the home directory to the
                        pin directory
        """
        self.setupOutputDir(outDir)
        self.vtune = vtune
        self.pin = os.path.join(HOME, pinLocalPath, "pin")
        self.micaLib = os.path.join(HOME, pinLocalPath,
                "source/tools/MICA-Pausable/obj-intel64/mica.so")
        self.micaSpec = os.path.join(HOME, pinLocalPath, 
                "source/tools/MICA-Pausable/itypes_custom.spec")
        self.uarchReportParser = UarchReportParser()
        self.cacheReportParser = CacheReportParser()

    def setupOutputDir(self, outDir):
        """
        @param the ouput directory
        This initializes the member varables of the class refering to output
        dirs
        postcondition: then creates the ouput dir and a standardized subdirectory 
                       structure in the output dir Strucutred as follows:
        # ROOT_OUT_DIR
        # - KernelOuts/ //outputs of kernel for true comparisons
        #   - Out/ //The outputs of the kernel for comparison to true
        # - Logs/ //the log of the command including the runtime
        #   - uarch.log
        # - Profiles/ //the raw vtune profile
        #   - Uarch
        # - Reports/ //reports generated from vtune
        """
        assert not os.path.exists(outDir), "Directory {} already exists! Please remove it before running this script".format(outDir)
        self.outDir = outDir
        self.profileDir = os.path.join(outDir, "Profiles")
        self.kernelOutputDir = os.path.join(outDir, "KernelOuts")
        self.logDir = os.path.join(outDir, "Logs")
        self.reportDir = os.path.join(outDir, "Reports")
        self.resultsDir = os.path.join(outDir, "Results")
        os.mkdir(self.outDir);
        os.mkdir(self.profileDir);
        os.mkdir(self.kernelOutputDir);
        os.mkdir(self.logDir);
        os.mkdir(self.reportDir);
        os.mkdir(self.resultsDir);

    def runVtuneUarch(self, app):
        '''
        initializes the uarch profile in profileDir/Uarch
        and a log in logDir/uarch.log
        and report in reportDir/uarchSum.txt
        final stats in resultsDir/uarchStats.txt
        '''
        execAndLog("{} -collect uarch-exploration -start-paused -data-limit=500 -result-dir {}/Uarch {} 2>&1 | tee {}/uarch.log".format(self.vtune, self.outDir, app, self.logDir))
        execAndLog("{} -report summary -inline-mode on -r {}/Uarch -report-output {}/uarchSum.txt".format(self.vtune, self.outDir, self.reportDir))
        #extract important stats from vtune report and write to the result dir
        stats = self.uarchReportParser.parseUarchReport(
                  os.path.join(self.reportDir,"uarchSum.txt"))
        with open(os.path.join(self.resultsDir, "uarchStats.txt"),'w') as oFile:
            oFile.write(stats);

    def getCacheCollectionMetrics(self):
        """
        This is a constant, it's just more readable here. These are the PMU
        events we need to pass to intel. (google intel PMU)
        """
        METRICS="MEM_INST_RETIRED.ALL_LOADS,"
        METRICS+="MEM_LOAD_RETIRED.L1_HIT,"
        METRICS+="MEM_LOAD_RETIRED.L1_MISS,"
        METRICS+="MEM_LOAD_RETIRED.FB_HIT,"
        METRICS+="MEM_LOAD_RETIRED.L2_HIT,"
        METRICS+="MEM_LOAD_RETIRED.L2_MISS,"
        METRICS+="MEM_LOAD_RETIRED.L3_HIT,"
        METRICS+="MEM_LOAD_RETIRED.L3_MISS,"
        METRICS+="MEM_LOAD_L3_MISS_RETIRED.LOCAL_DRAM,"
        return METRICS

    def getCacheCollectionFlags(self):
        """
        This is a constant, it's just more readable here. These are flags for a
        cache analysis
        """
        METRICS=self.getCacheCollectionMetrics()
        FLAGS="-collect-with runsa \
               -knob event-config={} \
               -knob process-kernel-binaries=true \
               -knob stack-size=16384 \
               -knob max-region-duration=1000 \
               -knob enable-user-tasks=true \
               -finalization-mode=full \
               -inline-mode=on".format(METRICS)
        return FLAGS

    def runVtuneCache(self, app):
        """
        runs vtune profiling to collect cache statistics in profileDir/Cache
        log in logDir/cache.log
        report in reportDir/cachSum.txt
        final stats in resultsDir/cacheStats.txt
        """
        FLAGS=self.getCacheCollectionFlags()
        #run the profiling
        execAndLog("{} {} -result-dir {} {} 2>&1 | tee {}".format(
                        self.vtune,
                        FLAGS,
                        os.path.join(self.profileDir, "Cache"),
                        app,
                        os.path.join(self.logDir, "cache.log")))
        #generate a report summary. Note, I've tried -report output, for some
        #reason only > works
        execAndLog("{} -report summary -r {} > {}".format(
                self.vtune, 
                os.path.join(self.profileDir, "Cache"),
                os.path.join(self.reportDir, "cacheSum.txt")))
        stats = self.cacheReportParser.parseCacheReport(
                os.path.join(self.reportDir,"cacheSum.txt"))
        with open(os.path.join(self.resultsDir, "cacheStats.txt"),'w') as oFile:
            oFile.write(stats);

    

    def runPinInstrCount(self, app):
        """
        Runs MICA PINtool to count the instructions. It then outputs the 
        instructions 
        """
        micaOut=os.path.join(self.profileDir, "Mica")
        os.mkdir(micaOut)
        useExistingConf = os.path.isfile("mica.conf")
        #make a new conf if you need
        if not useExistingConf:
            with open("mica.conf", 'w') as ofile:
                ofile.write("analysis_type: itypes\n")
                ofile.write("interval_size: full\n")
                ofile.write("itypes_spec_file: {}\n".format(self.micaSpec))
        #run pin
        execAndLog("{} -t {} -- {}".format(self.pin, self.micaLib, app))
        # parse the cryptic mica file to get the outputs
        with open("itypes_full_int_pin.out", 'r') as iFile:
            data = iFile.read().split(" ")
            with open("instrCounts.txt", 'w') as oFile:
                countNames = ["Count", "Memory", "Control", "Scalar", 
                         "FpScalar", "Nop", "Register", "Vector"]
                for name, val in zip(countNames, data[:len(countNames)]):
                    oFile.write("{} {}\n".format(name, val))
        #move all the outputs
        os.rename("itypes_full_int_pin.out",
                os.path.join(micaOut,"itypes_full_int_pin.out"))
        os.rename("itypes_other_group_categories.txt",
                os.path.join(micaOut,"itypes_other_group_categories.txt"))
        os.rename("mica.log",
                os.path.join(micaOut,"mica.log"))
        #these I think are only produced on error, so probably not needed
        #os.rename("pin.log",
        #        os.path.join(micaOut,"pin.log"))
        #os.rename("mica_progress.txt", 
        #        os.path.join(micaOut,"mica_progress.txt"))
        os.rename("instrCounts.txt", 
                os.path.join(micaOut,"instrCounts.txt"))
        shutil.copy("mica.conf", os.path.join(micaOut,"mica.conf"))
        #if we made the conf file, delete it on exit to be clean
        if useExistingConf:
            os.remove("mica.conf")
        #copy the instruction counts to results
        shutil.copy(os.path.join(micaOut,"instrCounts.txt"), 
                os.path.join(self.resultsDir, "instrCounts.txt"))


def execAndLog(cmd):
    print("executing: {}".format(cmd))
    os.system(cmd)


if __name__ == "__main__":
    outDir="DummyOut"
    app="./Dummy/bin/dummy /data2/kaplannp/Genomics/Datasets/Kernels/Dummy"
    execAndLog("rm -rf {}".format(outDir))
    runner = Runner(outDir)
    runner.runVtuneCache(app)
    #runner.runPinInstrCount(app)
    #runner.runVtuneUarch(app)
    #outDir="GbwtOut"
    #app="./Gbwt/bin/gbwt /data2/kaplannp/Genomics/Datasets/Kernels/smallGbwt"
    #execAndLog("rm -rf {}".format(outDir))
    #runner = Runner(outDir)
    #runner.runPinInstrCount(app)
    #runner.runVtuneUarch(app)
#GSSW="./Gssw/bin/gssw /data2/kaplannp/Genomics/Datasets/Kernels/Gssw"
#GBV="./Gbv/bin/gbv /data2/kaplannp/Genomics/Datasets/Kernels/Gbv"
