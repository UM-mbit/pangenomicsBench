#!/bin/bash

if [[ -v KERNEL_DATA ]]; then
    # CPU
    #odgi/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth.lay -t 32 -P 2>&1 | tee pgsgd_cpu_profiling.log
    # GPU
    ncu --target-processes all --set full --kernel-name gpu_layout_kernel --launch-skip 13 --launch-count 4 -o profile_results odgi/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth_gpu.lay -t 32 --gpu -P
    ncu --import profile_results.ncu-rep --metrics sm__warps_active.avg.pct_of_peak_sustained_active,smsp__thread_inst_executed_pred_on_per_inst_executed.ratio,gpu__compute_memory_throughput.avg.pct_of_peak_sustained_elapsed
else
    echo "environment variable KERNEL_DATA not set"
fi
