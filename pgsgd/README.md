# Instructions for running PGSGD benchmark

1. Git clone this repo: `$ git clone git@github.com:UM-mbit/pangenomicsBench.git`
2. Move into `pgsgd` directory: `$ cd pangenomicsBench/pgsgd`
3. Get odgi submodule: `$ git submodule update --init --recursive deps/odgi`
4. Build ODGI (tested with cmake v3.28.3 and g++ v13.2.0); ODGI's dependencies are listed in its [repo](https://github.com/pangenome/odgi?tab=readme-ov-file#building-from-source): `$ cd deps/odgi; mkdir build; cd build; cmake ..; make -j; cd ../../..`
5. Add libodgi to library path `$ export LD_LIBRARY_PATH=${PWD}/deps/odgi/lib:$LD_LIBRARY_PATH`
5. Set vtune variable `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`
6. Build pgsgd benchmark binary (note: use g++-11 compiler): `$ mkdir -p bin; make -j`
7. ~~Get preprocess pangenomes (chr20 takes some time; currently deactivated): `$ ./get_input.sh`~~
8. Run benchmark: `$ bin/pgsgd <thread-cnt> <path/input.og> <path/output.lay>`
9. Create png with ODGI: `$ deps/odgi/bin/odgi draw -i <path/input.og> -c <path/layout.lay> -p <path/fig.png> -C`


# Instruction for generating reference output

1. Build ODGI; see instructions above.
2. Run odgi layout (from pgsgd directory; set number of CPU threads `<NTRHEADS>`): `$ deps/odgi/bin/odgi layout -i data/DRB1-3123.og -o out_odgi.lay -t <NTHREADS> --gpu`
9. Create png with ODGI: `$ deps/odgi/bin/odgi draw -i data/DRB1-3123.og -c out_odgi.lay -p drb1_odgi.png -C`


# Profiling

* Run from `pangenomicBench` directory (parent dir of `pgsgd`): `python mainRun.py`
