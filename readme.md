# PangenomicsBench
This is a benchmark suite of pangenomics kernels from popular tools.
## Setup
1. Clone the repository and all submodules with 
   `git clone --recursive git@github.com:UM-mbit/pangenomicsBench.git`
2. Install the profiling tools. Needed for profiling analysis.
   `cd ProfileScripts && bash build.sh && cd ..`
3. Set local environment variables:
   + `VTUNE_HOME` - Path to the VTune installation directory. e.g.
     `/opt/intel/oneapi/vtune/latest`
   + `KERNEL_DATA` - Path to the kernel data directory.
4. Build the kernels by running the build script. If the build script fails,
   enter individual kernel directories, read the readme, and attempt manual
   build. Note, the compiler used in our paper is specified in the Makefile.
   Others may be used, but are untested.
   `bash build.sh`
