# Instructions for running transclosure benchmark

1. Git clone this repo: `$ git clone git@github.com:UM-mbit/pangenomicsBench.git`
2. Move into `transclosure` directory: `$ cd pangenomicsBench/transclosure`
3. Get seqwish submodule: `$ git submodule update --init --recursive deps/seqwish`
4. Build seqwish (tested with cmake v3.26.0 and g++ v11.4.0): `$ cd deps/seqwish; cmake -S . -B build && cmake --build build -- -j 10; cd ../..`
5. Add libseqwish to library path `$ export LD_LIBRARY_PATH=${PWD}/deps/seqwish/lib:$LD_LIBRARY_PATH`
5. Set vtune variable `export VTUNE_HOME=/opt/intel/oneapi/vtune/latest`
6. Build transclosure benchmark binary (note: use g++-11 compiler): `$ mkdir -p bin; make -j`
8. Run benchmark: `$ bin/transclosure <thread-cnt> <path/input.fasta> <path/input.paf> <path/output.gfa>`


# Visualize graph with odgi

1. Build ODGI; see pgsgd benchmark.
2. Create odgi pangenome graph from gfa: `$ odgi build -g <input.gfa> -o <output.og> -t <thread-count> -P`
2. Run odgi layout: `$ odgi layout -i <input.og> -o <layout.lay> -t <thread-count> --gpu`
9. Create png with ODGI: `$ odgi draw -i <input.og> -c <layout.lay> -p <image.png> -C`


# Profiling

* Run from `pangenomicBench` directory (parent dir of `transclosure`): `python mainRun.py`
