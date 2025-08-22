# Install instructions

1. Create a conda environment and install PGGB into it (this automatically installs wfmash, seqwish, smoothXG):
```
conda create --name pggb_env
conda activate pggb_env
conda install -c bioconda -c conda-forge pggb=0.7.2
```
2. Build odgi `../../Code/odgi`
```
pushd ../../Code/odgi
cmake -S . -B build
cmake --build build -- -j
popd
```


# Run tools

## PGGB
`pggb -i ../../Data/PggbData/chr20.pan.fasta -o out -t 56 -p 99.95 -s 10000 -A 2>&1 | tee out_pggb.log`

## odgi
```
(/usr/bin/time -v ../../Code/odgi/bin/odgi build -g out/*smooth.final.gfa -o chr20.og -t 54 -P 2>&1 ) | tee out_odgi.log
(/usr/bin/time -v ../../Code/odgi/bin/odgi layout -i chr20.og -o chr20.og.lay -T out/*.tsv -t 56 -P 2>&1 ) | tee -a out_odgi.log
(/usr/bin/time -v ../../Code/odgi/bin/odgi draw -i chr20.og -c chr20.og.lay -p chr20.png -C 2>&1 ) | tee -a out_odgi.log
```

# Timing data extraction
The run times of wfmash, seqwish, and smoothXG are extracted from `out_pggb.log`. PGGB shows in its output (besides the output of the different tools) the run times of the tools after their completion.
Look for lines like this:
```
...
wfmash -s 10000 -l 50000 -p 99.95 -n 1 -k 19 -H 0.001 -Y # -t 56 --tmp-base out ../../Data/PggbData/chr20.pan.fasta --lower-triangular --hg-filter-ani-diff 30 --approx-map  <- command with arguments
910.98s user 9.28s system 1110% cpu 82.83s total 4771084Kb max memory                                                                                                        <- run time diagnostics
...
```
First, PGGB prints the exact command (here wfmash) with all its arguments it ran. In the second line, it prints the run time diagnostics. The elapsed time is printed in seconds after "cpu" and before the memory information.
PGGB runs wfmash in two sequential steps; their elapsed time need to be summed up. Afterwards it runs seqwish and smoothXG.

The run time of odgi layout is recorded separately in `out_odgi.log`. The elapsed wall clock time needs to be summed up from all three substeps.
