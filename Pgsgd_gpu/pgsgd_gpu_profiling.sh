#!/bin/bash

if [[ -v KERNEL_DATA ]]; then
    # CPU
    #odgi/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth.lay -t 32 -P 2>&1 | tee pgsgd_cpu_profiling.log
    # GPU
    ncu --target-processes all --set full --kernel-name gpu_layout_kernel -o profile_results -f odgi/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth_gpu.lay -t 32 --gpu -P
    #python analyze_profile_results.py | tee analyze.log
else
    echo "Environment variable KERNEL_DATA not set!"
fi
