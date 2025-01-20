# Building GBWT
1. Ensure that the environment variable `VTUNE_HOME` points to the vtune
   directory (e.g. `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`). This is
   tested with g++-9
2. Make the sdsl dependency:
   ```
   cd deps/sdsl-lite
   BUILD_PORTABLE=1 CXXFLAGS="-O3 -ggdb -g3 -fopenmp" ./install.sh $(pwd)
   cd ../..
   ```
3. Make the Gbwt dependency
   `cd deps/gbwt && make -j && cd ../..`
4. Make the project
   `make -j`

# Running GBWT
The GBWT kernel is run with the command
`./bin/gbwt <path to dataset> <num iterations (optional)>`

e.g.

`./bin/gbwt Kernels/Gbwt/McFullGraph1e7 30`

# Generating the Input
These inputs are generated from the MinigraphCactus pagnenome graph from HPRC V1
for chromosome 20. They are random subsamplings of haplotype paths through the
graph (queries.txt). This uses the preprocess script included. using low query
length of 1 and high of 100. With paths extracted like this
`vg paths -x hprc-v1.0-mc-chm13.xg -A -H -g hprc-v1.0-mc-chm13.gbwt > extractedPaths.gaf`
We also download the gbwt file for that pangenome.
    

# Understanding Outputs
Outputs are of the format:
```
<nodeId>: (<startRange>,<endRange>)
```

One for each call to the kernel. The range is the range attribute of a
SearchState object defined in gbwt/algorithms.h.
This corresponds to the range of edges that can be followed from the node
