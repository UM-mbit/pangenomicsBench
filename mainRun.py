'''
This script runs all the kernels with profiling if desired (see below).
This script requires that $KERNEL_DATA is defined in the environment to point to
kernel data directory. Also ensure that for profiling, the machine is not 
running.

runVanillaApp: just runs the application for timing.
runVtuneUarch: collects uarch events with vtune.
runVtuneCache: collects cache events with vtune.
runPinInstrCount: collects instruction count with Intel PIN. This is slower and 
                  often is run on another dataset

In general. Any of these applications can be run as:
    `./application $KERNEL_DATA/application/dataset [optional. number of iterations to run]`
    If the number of iterations is specified, then the kernel will quit early 
    after processing that many inputs. This allows the user to modify the 
    duration of the run without changing the dataset.
'''
from ProfileScripts.runBenchmark import *
import shutil
from multiprocessing import Process
import os

'''
Set these to true to run the corresponding analysis. Uarch and Cache run the 
application once each, so enabling both triples execution time.
PIN can increase runtime by a larger factor.
'''
RUN_VANILLA = True
RUN_UARCH = False
RUN_CACHE = False
RUN_PIN = False

#useful variables
cwd = os.getcwd()
pinRuns = []
assert "KERNEL_DATA" in os.environ, "Please set KERNEL_DATA to the kernel data directory"

#Prepare output directory
os.system("rm -rf AllRunsOut")
os.system("mkdir AllRunsOut")

#load time: 280242171us
#kernel time: 28096616us
#write time: 549466us
gsswRunner = Runner("AllRunsOut/Gssw")
if RUN_VANILLA : gsswRunner.runVanillaApp("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McSubsampledShortReads".format(cwd))
if RUN_UARCH: gsswRunner.runVtuneUarch("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McSubsampledShortReads".format(cwd))
if RUN_CACHE: gsswRunner.runVtuneCache("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McSubsampledShortReads".format(cwd))
pinRuns.append(lambda:gsswRunner.runPinInstrCount("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McSubsampledShortReads".format(cwd)))

#load time: 27095355us
#kernel time: 23283850us
#write time: 1846884us
gbvRunner = Runner("AllRunsOut/Gbv")
if RUN_VANILLA : gbvRunner.runVanillaApp("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/McSubsampledLongReads".format(cwd))
if RUN_UARCH: gbvRunner.runVtuneUarch("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/McSubsampledLongReads".format(cwd))
if RUN_CACHE: gbvRunner.runVtuneCache("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/McSubsampledLongReads".format(cwd))
pinRuns.append(lambda:gbvRunner.runPinInstrCount("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/McSubsampledLongReads".format(cwd)))

#load time: 292282215us
#kernel time: 35296243us
#write time: 6166688us
gbwtRunner = Runner("AllRunsOut/Gbwt")
if RUN_VANILLA : gbwtRunner.runVanillaApp("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/Gbwt1e7".format(cwd))
if RUN_UARCH: gbwtRunner.runVtuneUarch("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/Gbwt1e7".format(cwd))
if RUN_CACHE: gbwtRunner.runVtuneCache("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/Gbwt1e7".format(cwd))
pinRuns.append(lambda:gbwtRunner.runPinInstrCount("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/Gbwt1e7".format(cwd)))

#load time: 9258875us
#kernel time: 29103560us
#write time: 669us
#Don't expect runtime to scale linearly. Some iterations are MUCH slower than others.
gwfaRunner = Runner("AllRunsOut/Gwfa")
if RUN_VANILLA : gwfaRunner.runVanillaApp("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaChrom".format(cwd))
if RUN_UARCH: gwfaRunner.runVtuneUarch("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaChrom".format(cwd))
if RUN_CACHE: gwfaRunner.runVtuneCache("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaChrom".format(cwd))
pinRuns.append(lambda:gwfaRunner.runPinInstrCount("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaChrom 60".format(cwd)))

#load time: 9258875us
#kernel time: 29103560us
#write time: 669us
#Don't expect runtime to scale linearly. Some iterations are MUCH slower than others.
gwfaRunnerLr = Runner("AllRunsOut/GwfaLr")
if RUN_VANILLA : gwfaRunnerLr.runVanillaApp("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaLr".format(cwd))
if RUN_UARCH: gwfaRunnerLr.runVtuneUarch("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaLr".format(cwd))
if RUN_CACHE: gwfaRunnerLr.runVtuneCache("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaLr".format(cwd))
pinRuns.append(lambda:gwfaRunnerLr.runPinInstrCount("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/GwfaLr 1000".format(cwd)))

#we run these in parallel because they are very slow, and their results aren't
#affected if the machine is running other processes
if RUN_PIN:
    processes = []
    for run in pinRuns:
        p = Process(target = run)
        p.start()
        processes.append(p)
    for p in processes:
        p.join()

os.system("bash checkAllRuns.sh AllRunsOut")
