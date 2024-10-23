'''
This code mainly just calls functions from the profiling scripts, and it adds a
few functions specific to genomics bench
'''
from ProfileScripts.runBenchmark import *
import shutil
from multiprocessing import Process
import os

class RunParams():
    def __init__(self, profApp, threadedApp, name, smallApp=None):
        self.profApp = profApp
        self.threadedApp = threadedApp
        self.name = name
        self.smallApp = smallApp
        self.runner = Runner("AllRunsOut/{}".format(name))

os.system("rm -rf AllRunsOut")
os.system("mkdir AllRunsOut")

runParams = [
        #RunParams(
        #  "./Dummy/bin/dummy.prof /data2/kaplannp/Genomics/Datasets/Kernels/Dummy",
        #  "OMP_NUM_THREADS={} ./Dummy/bin/dummy.omp /data2/kaplannp/Genomics/Datasets/Kernels/Dummy",
        #  "Dummy"),
        #RunParams(
        #  "./Dummy/bin/dummy.prof /data2/kaplannp/Genomics/Datasets/Kernels/Dummy",
        #  "OMP_NUM_THREADS={} ./Dummy/bin/dummy.omp /data2/kaplannp/Genomics/Datasets/Kernels/Dummy",
        #  "Dummy"),
        #default takes 26 seconds with 111396 lines, so pin is cut to 2.6s ideal
        #Note that the Gssw load is particularly bad, this makes the pin 
        #infeasible even if we do less iterations. My solution is a hacked first
        #1e3 inputs file
        RunParams(
          "./Gssw/bin/gssw.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
          "OMP_NUM_THREADS={} ./Gssw/bin/gssw.omp /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
          "GsswMc",
          "{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e3Inputs".format((os.getcwd()))),
        #default takes 122 seconds with 5000 reads. Downsize 1000-vtune 125-pin
        RunParams(
          "./Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 1000",
          "OMP_NUM_THREADS={} ./Gbv/bin/gbv.omp $KERNEL_DATA/Gbv/Mc1e3Reads 1000",
          "GbvMc",
          "{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 125".format(os.getcwd())),
        #default seems to get stuck. Hours. 10 takes .26s. 100 takes 474s. Try50
        #we do 10 for pin
        #RunParams(
        #  "./Gbv/bin/gbv.prof /data2/kaplannp/Genomics/Datasets/Gbv/Pggb1e3Reads 50",
        #  "OMP_NUM_THREADS={} ./Gbv/bin/gbv.omp /data2/kaplannp/Genomics/Datasets/Gbv/Pggb1e3Reads 50",
        #  "GbvPggb",
        #  "{}/Gbv/bin/gbv.prof /data2/kaplannp/Genomics/Datasets/Gbv/Pggb1e3Reads 10".format(os.getcwd())),
        #default takes 12 seconds with 6400000 lines, so pin is cut to 1.2s
        #RunParams(
        #  "{}/Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/MinigraphCactus1e5Query_1To40Range_42Seed".format(os.getcwd()),
        #  "OMP_NUM_THREADS={} ./Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/MinigraphCactus1e5Query_1To40Range_42Seed",
        #  "GbwtMc",
        #  "{}/Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/MinigraphCactus1e5Query_1To40Range_42Seed 640000".format(os.getcwd())),
        #default takes 38 seconds with 26000000 lines (note this is one more zero than the other gbwt). we cut it to 1.9s
        #RunParams(
        #  "./Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/Pggb1e5Query_1To40Range_42Seed",
        #  "OMP_NUM_THREADS={} ./Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/Pggb1e5Query_1To40Range_42Seed",
        #  "GbwtPggb",
        #  "{}/Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/Pggb1e5Query_1To40Range_42Seed 1300000".format(os.getcwd())),
        #RunParams(
        #  "./Gwfa/bin/gwfa.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13 6000",
        #  "OMP_NUM_THREADS={} ./Gwfa/bin/gwfa.omp /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13",
        #  "GwfaPggbChm13",
        #  "./Gwfa/bin/gwfa.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13 5463"),
        #default takes a hot second. 60 takes 30s.
        RunParams(
          "./Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 60",
          "OMP_NUM_THREADS={} ./Gwfa/bin/gwfa.omp $KERNEL_DATA/Gwfa/McChm13 60",
          "GwfaMcChm13",
          "{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 10".format(os.getcwd()))
        ]

#first we run all the vtune and timing profiling sequentially
for runParam in runParams:
    try:
        threadRange = [1] + list(range(8, 65, 8))
        runParam.runner.runVanillaApp(runParam.profApp)
        runParam.runner.runVtuneUarch(runParam.profApp)
        runParam.runner.runVtuneCache(runParam.profApp)
        runParam.runner.runThreadScaling(runParam.threadedApp, threadRange)
    except:
        with open("failed.log", "a") as f:
            f.write("Failed to run {}\n".format(runParam.name))

#theen we run PIN
#we run these in parallel because they are very slow, and they don't require
#clean machine for profiling
processes = []
for runParam in runParams:
    p = Process(
            target = lambda:runParam.runner.runPinInstrCount(runParam.smallApp))
    p.start()
    processes.append(p)
for p in processes:
    p.join()

# combined transclosure
#outDir="out"
#execAndLog("rm -rf {}".format(outDir))
#runner = Runner(outDir)
#app="./transclosure/bin/transclosure 8 /data2/jnms/Shared/HPRCv2_chr20/chr20.pan.fasta /data2/jnms/Shared/HPRCv2_chr20/chr20.paf chr20.gfa"
#runner.runVanillaApp(app, outputs=["chr20.gfa"])
#app="./transclosure/bin/transclosure.prof 8 /data2/jnms/Shared/HPRCv2_chr20/chr20.pan.fasta /data2/jnms/Shared/HPRCv2_chr20/chr20.paf chr20.gfa"
#runner.runVtuneUarch(app)
#runner.runVtuneCache(app)
#app="./transclosure/bin/transclosure.prof {} /data2/jnms/Shared/HPRCv2_chr20/chr20.pan.fasta /data2/jnms/Shared/HPRCv2_chr20/chr20.paf chr20.gfa"
#runner.runThreadScaling(app, [1] + [4] + list(range(8, 65, 8)))
#app="/data2/jnms/pangenomicsBench/transclosure/bin/transclosure.prof 1 /data2/jnms/Shared/HPRCv2_chr20/chr20.pan.fasta /data2/jnms/Shared/HPRCv2_chr20/chr20.paf chr20.gfa"
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
#app="/data2/jnms/pangenomicsBench/pgsgd/bin/pgsgd.prof 1 /data2/jnms/Shared/DRB1-3123/DRB1-3123.og out_benchmark.lay"
#runner.runPinInstrCount(app)
#runner.printResults()
