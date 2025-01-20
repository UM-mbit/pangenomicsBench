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
RUN_UARCH = False
RUN_CACHE = False
RUN_PIN = False

#useful variables
cwd = os.getcwd()
pinRuns = []

#Prepare output directory
os.system("rm -rf AllRunsOut")
os.system("mkdir AllRunsOut")

#load time: 280242171us
#kernel time: 28096616us
#write time: 549466us
gsswRunner = Runner("AllRunsOut/Gssw")
gsswRunner.runVanillaApp("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e4Inputs".format(cwd))
if RUN_UARCH: gsswRunner.runVtuneUarch("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e4Inputs".format(cwd))
if RUN_CACHE: gsswRunner.runVtuneCache("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e4Inputs".format(cwd))
pinRuns.append(lambda:gsswRunner.runPinInstrCount("{}/Gssw/bin/gssw.prof $KERNEL_DATA/Gssw/McFirst1e3Inputs".format(cwd)))

#load time: 27095355us
#kernel time: 23283850us
#write time: 1846884us
gbvRunner = Runner("AllRunsOut/Gbv")
gbvRunner.runVanillaApp("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 1000".format(cwd))
if RUN_UARCH: gbvRunner.runVtuneUarch("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 1000".format(cwd))
if RUN_CACHE: gbvRunner.runVtuneCache("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 1000".format(cwd))
pinRuns.append(lambda:gbvRunner.runPinInstrCount("{}/Gbv/bin/gbv.prof $KERNEL_DATA/Gbv/Mc1e3Reads 100".format(cwd)))

#load time: 292282215us
#kernel time: 35296243us
#write time: 6166688us
gbwtRunner = Runner("AllRunsOut/Gbwt")
gbwtRunner.runVanillaApp("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/McFullGraph1e7 5000000".format(cwd))
if RUN_UARCH: gbwtRunner.runVtuneUarch("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/McFullGraph1e7 5000000".format(cwd))
if RUN_CACHE: gbwtRunner.runVtuneCache("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/McFullGraph1e7 5000000".format(cwd))
pinRuns.append(lambda:gbwtRunner.runPinInstrCount("{}/Gbwt/bin/gbwt.prof $KERNEL_DATA/Gbwt/McFullGraph1e7 500000".format(cwd)))

#load time: 9258875us
#kernel time: 29103560us
#write time: 669us
#Don't expect runtime to scale linearly. Some iterations are MUCH slower than others.
gwfaRunner = Runner("AllRunsOut/Gwfa")
gwfaRunner.runVanillaApp("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 60".format(cwd))
if RUN_UARCH: gwfaRunner.runVtuneUarch("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 60".format(cwd))
if RUN_CACHE: gwfaRunner.runVtuneCache("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 60".format(cwd))
pinRuns.append(lambda:gwfaRunner.runPinInstrCount("{}/Gwfa/bin/gwfa.prof $KERNEL_DATA/Gwfa/McChm13 30".format(cwd)))

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
