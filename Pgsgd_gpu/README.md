# PGSGD-GPU

* We will profile the GPU version of PGSGD when running the standard odgi layout with the `--gpu` argument.
* We need to build odgi with GPU support (for further instructions, see odgi's readme):
  * Point the environment variable `CUDA_HOME` to your CUDA installation and add its `bin` and `lib64` directories to your `PATH` and `LD_LIBRARY_PATH` respectively.
  * Get odgi and its submodules: `$ git submodule update --init --recursive .`
  * Enter odgi's directory in `Pgsgd_gpu`: `$ cd odgi`
  * Run cmake and build odgi: `$ cmake -DUSE_GPU=ON -H. -Bbuild && cmake --build build -- -j 32`
* Run profiling:
  * Set environment variable `KERNEL_DATA` accordingly.
  * Run profiling script `pgsgd_gpu_profiling.sh`: `$ bash pgsgd_gpu_profiling.sh`
