# Building Gssw
1. Ensure that the environment variable `VTUNE_HOME` points to the vtune
   directory (e.g. `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`). This is
   tested with the compiler shipped in the GraphAligner conda environment
   (g++-11)
2. Create python 3.8 environment. Python of later version could cause the
   program to fail to build.
   `conda create -n vg python=3.8`
   activate the environment
3. Build Gssw library
   `cd deps/Gssw && make -j && cd ../..`
4. Build the kernel
   `make -j`

# Running the Kernel
Run the kernel with the command
`./bin/gssw <path to data> <num iterations, or blank>`
Example with `$KERNEL_DATA` pointing to the kernel data directory
`./bin/gssw $KERNEL_DATA/Gssw/McFirst1e4Inputs 30`:

# Generating Inputs
Run the command
`vg map -d <path to chr20 + header with gcsa, pg (or vg), and gcsa.lcp files> 
-t 1 -F <path to reads.fa>`
An example
`vg map -d graphDir/chr20.hprc-v1.0-pggb -t 1 -F reads.fa`
The GraphDir directory should contain chr20.hprc-v1.0-pggb.pg, 
chr20.hprc-v1.0-pggb.gcsa, and chr20.hprc-v1.0-pggb.gcsa.lcp.

# Understanding Outputs
Each run of the kernel generates one line in the output file. The output is not
json. Each line has the format
```
<BestScore>@<FirstNodeOffset>:<GraphNode>[<CigarStringThroughGraphNodeSequence>]<GraphNode>[<CigarStringThroughGraphNodeSequence]...
```

TODO gotta update this when we hear from Dev.

