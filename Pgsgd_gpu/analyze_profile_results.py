import ncu_report
import numpy as np

report = ncu_report.load_report("profile_results.ncu-rep")
report = report[0]

d = np.zeros((30,3))

for i in range(len(report)):
    occupancy = report[i]['sm__warps_active.avg.pct_of_peak_sustained_active'].value()
    warp_utilization = report[i]['smsp__thread_inst_executed_pred_on_per_inst_executed.ratio'].value() * 100.0 / 32.0
    mem_throughput = report[i]['gpu__compute_memory_throughput.avg.pct_of_peak_sustained_elapsed'].value()

    d[i] = [occupancy, warp_utilization, mem_throughput]

print("Occupancy, Warp Utilization, Memory Throughput")
print(d)

mean_glob = np.mean(d, axis=0)
print("\nGlobal:")
print("Mean Occupancy:", mean_glob[0])
print("Mean Warp Utilization:", mean_glob[1])
print("Mean Memory Throughput:", mean_glob[2])

mean_block0 = np.mean(d[:15], axis=0)
print("\nIteration 1-15:")
print("Mean Occupancy:", mean_block0[0])
print("Mean Warp Utilization:", mean_block0[1])
print("Mean Memory Throughput:", mean_block0[2])

mean_block1 = np.mean(d[15:], axis=0)
print("\nIteration 16-30:")
print("Mean Occupancy:", mean_block1[0])
print("Mean Warp Utilization:", mean_block1[1])
print("Mean Memory Throughput:", mean_block1[2])
