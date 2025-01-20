# PangenomicsBench
This is a benchmark suite of pangenomics kernels from popular tools.
It includes the following kernels:
| Kernel Name | Toolchain     | Type of Kernel          | Summary                                                                                      |
|-------------|---------------|-------------------------|----------------------------------------------------------------------------------------------|
| GSSW        | Vg Map        | 2D dynamic programming. Alignment | Farrar's style SIMD parallelization with POA-style lookups to handle the graph                |
| GBV         | GraphAlginer  | 2D dynamic programming. Alignment | Myers bitvector extended to graph. Uses a priority queue to order computations, allowing alignment to cyclic graphs |
| GBWT        | Vg Giraffe    | Table lookups. Indexing | GBWT index find query. Used to extend clustered seed hits in Giraffe.                        |
| GWFA        | Minigraph     | 2D dynamic programming. Alignment | Used in Minigraph for alignment and graph building. Although GWFA is used in the chaining step to link seed anchors, computationally, it resembles a dynamic

## Setup
1. Clone the repository and all submodules with 
   `git clone --recursive git@github.com:UM-mbit/pangenomicsBench.git`
2. Install the profiling tools. Needed for profiling analysis.
   `cd ProfileScripts && bash build.sh && cd ..`
3. Set local environment variables:
   + `VTUNE_HOME` - Path to the VTune installation directory. e.g.
     `/opt/intel/oneapi/vtune/latest`
   + `KERNEL_DATA` - Path to the kernel data directory.
4. Build the kernels by running the build script. It will print build status of
   kernels at the end of the script. If a kernel fails
   enter individual kernel directories, read the README, and attempt manual
   build. Note, the compiler used in our paper is specified in the Makefile.
   Others may be used, but are untested.
   `bash build.sh`
5. Run the kernels with the run script.
   `python mainRun.py`
   By default the script will run the kernels once with timing collection.
   To run with other profiling options read the header comment of `mainRun.py`
   Kernels can also be run individually. Usually as:
   `./bin/kernel.prof <path to input> [optional: numIterations]`
   numIterations can be used to stop after `n` inputs to control the duration
   without modifying the dataset.
   See the README for each kernel for more information.
