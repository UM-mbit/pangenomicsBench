# PGSGD Kernel

## Dependencies

The dependencies of the PGSGD kernel matches odgi's dependencies
([see](https://github.com/nsmlzl/odgi/tree/621638640bf812281b63f34fce37d9d5235ffb5d?tab=readme-ov-file#building-from-source)), 
which consist out of common C++ build tools (gcc, cmake, ...).

## Build instructions for PGSGD kernel

1. Git clone this repo: `$ git clone git@github.com:UM-mbit/pangenomicsBench.git`
2. Move into `pgsgd` directory: `$ cd pangenomicsBench/pgsgd`
3. Get odgi submodule: `$ git submodule update --init --recursive deps/odgi`
4. Build ODGI (tested with cmake v3.28.3 and g++ v11.4.0); ODGI's dependencies are listed in its [repo](https://github.com/pangenome/odgi?tab=readme-ov-file#building-from-source): `$ cd deps/odgi; cmake -S . -B build && cmake --build build -- -j 10; cd ../..`
5. Add libodgi to library path `$ export LD_LIBRARY_PATH=${PWD}/deps/odgi/lib:$LD_LIBRARY_PATH`
5. Set vtune variable `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`
6. Build pgsgd benchmark binary: use g++-11 compiler): `$ mkdir -p bin; make -j`
7. Optional: Run benchmark manually: `$ bin/pgsgd <thread-cnt> <path/input.og> <path/output.lay>`.
   A png for the layout can be generated with ODGI: `$ deps/odgi/bin/odgi draw -i <path/input.og> -c <path/layout.lay> -p <path/fig.png> -C`
