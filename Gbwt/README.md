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
    

# Understanding Outputs
Outputs are of the format:
```
<nodeId>: (<startRange>,<endRange>)
```

One for each call to the kernel. The range is the range attribute of a
SearchState object defined in gbwt/algorithms.h.
This corresponds to the range of edges that can be followed from the node
