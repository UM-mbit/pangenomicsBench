#!/bin/bash

if [[ -v KERNEL_DATA ]]; then
    # Run time analysis CPU vs GPU
    # CPU (--gpu argument utilizes the cache-optimized CPU implementation)
    echo "*** CPU run time profiling"
    /usr/bin/time -v deps/odgi_cpu/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth_cpu.lay -t 32 --gpu -P
    # GPU
    echo "*** GPU run time profiling"
    /usr/bin/time -v deps/odgi_gpu/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth_gpu.lay -t 32 --gpu -P

    # Microarchitecture analysis GPU
    echo "*** GPU microarchitecture profiling"
    ncu --target-processes all --set full --kernel-name gpu_layout_kernel -o profile_results -f deps/odgi_gpu/bin/odgi layout -i $KERNEL_DATA/Pgsgd/chr20_smooth.og -o chr20_smooth_gpu.lay -t 32 --gpu -P
else
    echo "Environment variable KERNEL_DATA not set!"
fi
