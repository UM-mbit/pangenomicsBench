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
        #  "./Gssw/bin/gssw.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactusChopped8_1e4Reads",
        #  "OMP_NUM_THREADS={} ./Gssw/bin/gssw.omp /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
        #  "GsswMcChopped",
        #  "{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e3Inputs".format((os.getcwd()))),

        #RunParams(
        #  "./Gssw/bin/gssw.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
        #  "OMP_NUM_THREADS={} ./Gssw/bin/gssw.omp /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
        #  "GsswMcFull",
        #  "{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e3Inputs".format((os.getcwd()))),
        #RunParams( 
        #    "./Bsw/bsw.prof -pairs $KERNEL_DATA/Bsw/HprcIllumina/mid/queries.txt -t 1 -b 512",
        #    "./Bsw/bsw.prof -pairs $KERNEL_DATA/Bsw/HprcIllumina/mid/queries.txt -t {} -b 512",
        #    "Bsw",
        #    "{}/Bsw/bsw.prof -pairs $KERNEL_DATA/Bsw/HprcIllumina/small/queries.txt -t 1 -b 512".format(os.getcwd())),
        #RunParams( 
        #    "./Bsw/bsw.prof -pairs $KERNEL_DATA/Bsw/GenomicsBench/large/bandedSWA_SRR7733443_1m_input.txt -t 1 -b 512",
        #    "./Bsw/bsw.prof -pairs $KERNEL_DATA/Bsw/GenomicsBench/large/bandedSWA_SRR7733443_1m_input.txt -t {} -b 512",
        #    "BswOldData",
        #    "{}/Bsw/bsw.prof -pairs $KERNEL_DATA/Bsw/GenomicsBench/small/bandedSWA_SRR7733443_100k_input.txt -t 1 -b 512".format(os.getcwd())),
        #RunParams(
        #  "./Gssw/bin/gssw.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
        #  "OMP_NUM_THREADS={} ./Gssw/bin/gssw.omp /data2/kaplannp/Genomics/Datasets/Kernels/Gssw/MinigraphCactus1e4Reads",
        #  "GsswMc",
        #  "{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e3Inputs".format((os.getcwd()))),
        #default takes 122 seconds with 5000 reads. Downsize 1000-vtune 125-pin
        #RunParams(
        #  "./Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 1000",
        #  "OMP_NUM_THREADS={} ./Gbv/bin/gbv.omp $KERNEL_DATA/Gbv/Mc1e3Reads 1000",
        #  "GbvMc",
        #  "{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 125".format(os.getcwd())),
        #Note the pin doesn't use this because it's so goddamn slow
        #RunParams(
        #  "{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/McFullGraph1e7".format(os.getcwd()),
        #  "OMP_NUM_THREADS={} ./Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/McFullGraph1e7",
        #  "GbwtMc",
        #  "{}/Gbwt/bin/gbwt.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gbwt/MinigraphCactus1e5Query_1To40Range_42Seed 640000".format(os.getcwd())),
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
        #default takes a hot second. 60 takes 30s.
        #RunParams(
        #  "./Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13",
        #  "OMP_NUM_THREADS={} ./Gwfa/bin/gwfa.omp $KERNEL_DATA/Gwfa/McChm13 60",
        #  "GwfaMcChm13",
        #  "{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 10".format(os.getcwd())),
        ##This one is fast at 5000, but gets stuck somewhere before 6000
        ##6000 takes 1hour 23 minutes
        #RunParams(
        #  "./Gwfa/bin/gwfa.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13",
        #  "OMP_NUM_THREADS={} ./Gwfa/bin/gwfa.omp /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13",
        #  "GwfaPggbChm13",
        #  "{}/Gwfa/bin/gwfa.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13 5463".format(os.getcwd())),
        #TODO: should move the data to the kernel directory
        #unfortunately, you must run the export function before running the
        #script
        #export LD_LIBRARY_PATH=$(pwd)/pgsgd/deps/odgi/lib:$LD_LIBRARY_PATH
        RunParams(
            "pgsgd/bin/pgsgd.prof 1 $KERNEL_DATA/Pgsgd/chr20.og out_benchmark.lay",
            "pgsgd/bin/pgsgd.prof {} $KERNEL_DATA/Pgsgd/chr20.og out_benchmark.lay",
            "Pgsgd",
            " {}/pgsgd/bin/pgsgd.prof 1 $KERNEL_DATA/Pgsgd/DRB1-3123.og out_benchmark.lay".format(os.getcwd())),
        #RunParams(
        #    "transclosure/bin/transclosure.prof 1 $KERNEL_DATA/Transclosure/chr20.pan.fasta $KERNEL_DATA/Transclosure/chr20.paf chr20.gfa",
        #    "transclosure/bin/transclosure.prof {} $KERNEL_DATA/Transclosure/chr20.pan.fasta $KERNEL_DATA/Transclosure/chr20.paf chr20.gfa",
        #    "Transclosure",
        #    "{}/transclosure/bin/transclosure.prof 1 /data2/jnms/Shared/cerevisiae/cerevisiae.pan.fa /data2/jnms/Shared/cerevisiae/cerevisiae.paf cerevisiae.gfa".format(os.getcwd())),
        ]

#sswRunner = Runner("AllRunsOut/SswFull")
#sswProfApp = "./Ssw/ssw $KERNEL_DATA/Bsw/HprcIllumina/midRandomized/queries.txt"
#sswPinApp = "{}/Ssw/ssw $KERNEL_DATA/Bsw/HprcIllumina/small/queries.txt".format(os.getcwd())
#sswRunner.runVanillaApp(sswProfApp)
#sswRunner.runVtuneUarch(sswProfApp)
#sswRunner.runVtuneCacheStall(sswProfApp)
#sswRunner.runVtuneCache(sswProfApp)
#sswRunner.runPinInstrCount(sswPinApp)


#gwfaMcRunner = Runner("AllRunsOut/GwfaMc")
#gwfaMcProfApp = "./Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13"
#gwfaMcRunner.runVanillaApp(gwfaMcProfApp)
#
#gwfaPggbRunner = Runner("AllRunsOut/GwfaPggb")
#gwfaPggbProfApp = "./Gwfa/bin/gwfa.prof /data2/kaplannp/Genomics/Datasets/Kernels/Gwfa/PggbChm13"
#gwfaPggbRunner.runVanillaApp(gwfaPggbProfApp)

#first we run all the vtune and timing profiling sequentially
for runParam in runParams:
    try:
        threadRange = [1, 8, 16, 32, 64]
        #runParam.runner.runVanillaApp(runParam.profApp)
        runParam.runner.runVtuneUarch(runParam.profApp)
        #runParam.runner.runVtuneCacheStall(runParam.profApp)
        #runParam.runner.runVtuneCache(runParam.profApp)
        #runParam.runner.runThreadScaling(runParam.threadedApp, threadRange)
    except:
        with open("failed.log", "a") as f:
            print("Failed to run {}\n".format(runParam.name))
            f.write("Failed to run {}\n".format(runParam.name))

#nervous about this. Untested.


#BswRunner.runVtuneCacheStall(bswProfApp)
#BswRunner.runVtuneCache(bswProfApp)


#theen we run PIN
#we run these in parallel because they are very slow, and they don't require
#clean machine for profiling
#processes = []
#for runParam in runParams:
    #def kernelLambda():
        #print("zkn {} started".format(runParam.name))
        #runParam.runner.runPinInstrCount(runParam.smallApp)
        #print("zkn {} complete".format(runParam.name))
    #p = Process(target = kernelLambda)
    #p.start()
    #processes.append(p)
#for p in processes:
    #p.join()
