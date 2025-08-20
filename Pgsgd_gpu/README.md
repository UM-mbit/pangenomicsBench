# PGSGD-GPU

This directory contains two odgi projects:
1. `deps/odgi_gpu` contains odgi layout with gpu acceleration.
2. `deps/odgi_cpu` contains odgi layout with the cache-optimized CPU layout implementation (included as reference for run time analysis).


## Dependencies

Building both projects requires odgi's dependencies ([see](https://github.com/pangenome/odgi/tree/be6a0202501d7ea2ac57f9ad89d4d10ed5dbd7c6?tab=readme-ov-file#building-from-source)).
Furthermore, CUDA and NVIDIA nsight compute are required on the test machine.
Lastly, the analysis requires a python environment with numpy and the python tools from nsight compute (see [profiling section](#profiling)).


## Build instructions for `deps/odgi_gpu`

We build odgi with GPU support (following odgi's readme):
1. Point the environment variable `CUDA_HOME` to your CUDA installation and add its `bin` and `lib64` directories to your `PATH` and `LD_LIBRARY_PATH` respectively.
2. Get all submodules: `$ git submodule update --init --recursive .`
3. Enter odgi-gpu's directory: `$ cd deps/odgi_gpu`
4. Run cmake and build odgi-gpu: `$ cmake -DUSE_GPU=ON -H. -Bbuild && cmake --build build -- -j`


## Build instructions for `deps/odgi_cpu`

Follow official instructions of odgi's README.


## Profiling

To run profiling:
1. Set environment variable `KERNEL_DATA` accordingly.
2. Run profiling and extract microarchitecture results:
```
(bash pgsgd_gpu_profiling.sh && python analyze_profile_results.py) 2>&1 | tee ../PGSGDGPUProfiling.txt
```
   Notes: The numpy package is needed for extracting microarchitecture results.
   Furthermore, the ncu-report package is required; you might need to include
   the python directory of nsight-compute in your PYTHONPATH (`export PYTHONPATH="$CUDA_HOME/nsight-compute-2024.1.0/extras/python:$PYTHONPATH"`).

The run time acceleration (odgi-gpu vs odgi-cpu) can be determined by comparing
the elapsed wall clock time in `PGSGDGPUProfiling.txt`.  PGSGD-GPUs profiling
results from Table 6 are reported under the "Global" section (to the end of
`PGSGDGPUProfiling.txt`. The other metrics discussed in the paper can be
extracted by opening `profile_results.ncu-rep` with nsight-compute.
