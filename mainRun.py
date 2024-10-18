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


# combined transclosure
#outDir="out"
#execAndLog("rm -rf {}".format(outDir))
#runner = Runner(outDir)
#app="./transclosure/bin/transclosure 8"
#runner.runVanillaApp(app, outputs=["chr20.gfa"])
#app="./transclosure/bin/transclosure.prof 8"
#runner.runVtuneUarch(app)
#runner.runVtuneCache(app)
#app="./transclosure/bin/transclosure.prof {}"
#runner.runThreadScaling(app, [1] + [4] + list(range(8, 65, 8)))
#app="./transclosure/bin/transclosure.prof 1"
#runner.runPinInstrCount(app)
#runner.printResults()

# combined pgsgd
#outDir="out"
#execAndLog("rm -rf {}".format(outDir))
#runner = Runner(outDir)
#app="./pgsgd/bin/pgsgd 8 /data2/jnms/Shared/HPRCv2_chr20/chr20.og out_benchmark.lay"
#runner.runVanillaApp(app, outputs=["out_benchmark.lay"])
#app="./pgsgd/bin/pgsgd.prof 8 /data2/jnms/Shared/HPRCv2_chr20/chr20.og out_benchmark.lay"
#runner.runVtuneUarch(app)
#runner.runVtuneCache(app)
#app="./pgsgd/bin/pgsgd.prof {} /data2/jnms/Shared/HPRCv2_chr20/chr20.og out_benchmark.lay"
#runner.runThreadScaling(app, [1] + [4] + list(range(8, 65, 8)))
#app="./pgsgd/bin/pgsgd.prof 1 /data2/jnms/Shared/HPRCv2_chr20/chr20.og out_benchmark.lay"
#runner.runPinInstrCount(app)
#runner.printResults()


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
