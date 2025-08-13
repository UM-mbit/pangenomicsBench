This contains two projects:
1. WFA2: leading CPU implementation of WFA library
2. TSUNAMI: Improved GPU implementation of WFA

# Building
1. build TSUNAMI. REQUIRES Cuda >= 11.8:
   `cd deps/TSUNAMI_PACT && make all && cd ../..`
2. build wfa2:
   `cd deps/WFA2-lib && cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build --verbose -j && cd ../..`

# Evaluation
## Full Runs (quickstart)
We include two scripts to run for all datasets used in the paper.

`bash runTiming.sh` will run timings compared to wfa2 (data used for figure 9 of
the paper, GPU vs CPU WFA Timing)

`bash runNcu.sh` will run TSUNAMI with NSIGHT Compute to get uarch
characteristics in Table VI of the paper (GPU Microarchitecture Utilization).
## Personalized runs
For users who wish to evaluate datasets other than those in the paper, the 
runTiming script can be used as a template. 
TSUNAMI operates on synthetic data generated from TSUNAMI's dataset
generator (which is derived with little modification from the benchmarking tool 
included in WFA2). It has a header which needs to be stripped for use with WFA2
(example in `runTiming.sh`).
