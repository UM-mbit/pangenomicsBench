#Run Tsunami timing analysis
cd GpuWfa
bash runTiming.sh | tee ../tsunamiTiming.txt
cd ..

# Run PGSGD-GPU profiling
cd Pgsgd_gpu
(bash pgsgd_gpu_profiling.sh && python analyze_profile_results.py) 2>&1 | tee ../PGSGDGPUProfiling.txt
cd ..
