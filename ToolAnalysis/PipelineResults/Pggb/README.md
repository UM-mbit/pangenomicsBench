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


# Generate timing breakdown

To generate the timing breakdown of the PGGB pangenome graph building pipeline stages, run the profiling script: 
```
bash runPGGBBreakdown.sh | tee pggb_breakdown.txt
```
The script reports for each stage the elapsed time in seconds. Stdout of each tool is send to a separate log file.
The commands with arguments from this script are extracted from PGGB.
