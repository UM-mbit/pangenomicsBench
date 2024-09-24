'''
This code mainly just calls functions from the profiling scripts, and it adds a
few functions specific to genomics bench
'''
from ProfileScripts.runBenchmark import *
import shutil
import os


class PangenomicsBenchRunner(Runner):
    '''
    subclass of the general runner class with pangenomicsBench specific commands
    '''

    def runVanillaApp(self, app):
        super().runVanillaApp(app)
        shutil.move("Out", os.path.join(self.kernelOutputDir, "Out"))


outDir="Outs/CacheUBenchmark"
#app="OMP_NUM_THREADS={} ./Dummy/bin/dummy.omp /data2/kaplannp/Genomics/Datasets/Kernels/Dummy"
app="./CacheUBenchmark/bin/cacheUBenchmark.prof"
execAndLog("rm -rf {}".format(outDir))
runner = Runner(outDir)
#runner.runThreadScaling(app, [1] + list(range(8, 65, 8)))
#runner.runVanillaApp(app, outputs="Out")
#runner.runPinInstrCount(app)
#runner.runVtuneUarch(app)
runner.runVtuneCache(app)
#runner.printResults()
