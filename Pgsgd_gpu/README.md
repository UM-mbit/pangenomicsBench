# Notes:

This directory contains two odgi projects:
1. `deps/odgi_cpu` contains odgi layout with cache-optimized layout implementation (build it like regular odgi without GPU).
2. `deps/odgi_gpu` contains odgi layout with gpu acceleration.

# PGSGD-GPU

* We will profile the GPU version of PGSGD when running the standard odgi layout with the `--gpu` argument.
* We need to build odgi with GPU support (for further instructions, see odgi's readme):
  * Point the environment variable `CUDA_HOME` to your CUDA installation and add its `bin` and `lib64` directories to your `PATH` and `LD_LIBRARY_PATH` respectively.
  * Get odgi and its submodules: `$ git submodule update --init --recursive .`
  * Enter odgi-gpu's directory in `Pgsgd_gpu`: `$ cd deps/odgi_gpu`
  * Run cmake and build odgi: `$ cmake -DUSE_GPU=ON -H. -Bbuild && cmake --build build -- -j 32`
* Run profiling:
  * Set environment variable `KERNEL_DATA` accordingly.
  * Run profiling script `pgsgd_gpu_profiling.sh`: `$ bash pgsgd_gpu_profiling.sh`
  * Extract profiling data by running `python analyze_profile_results.py`. The
    numpy package is needed. Furthermore, the ncu-report package is required;
    you might need to modify your PYTHONPATH (`export PYTHONPATH="$CUDA_HOME/nsight-compute-2024.1.0/extras/python:$PYTHONPATH"`).
    PGSGD-GPUs profiling results from Table 6 are reported as "Global". The other metrics discussed in the paper can be extracted
    by opening `profile_results.ncu-rep` with nsight-compute.
