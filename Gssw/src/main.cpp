#include <iostream>
#include <fstream>
#include <vector>
#include <omp.h>
#include <chrono>

#include "gssw.h"
#include "loadParams.h"
#include "eval.h"
#include "profilingUtils.h"

#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(int argc, char* argv[]){
  std::string inputDir = getInputDirFromArgs(argc, argv);
  int num_iter_override = getNumItersFromArgs(argc, argv);

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();

  init_output_dir(OUT_DIR);
  int num_inputs = ld_num_inputs(inputDir);
  int numIters = std::min(num_inputs, num_iter_override);
  std::vector<ReadAlignmentParams>* params =
      load_read_alignment_params(numIters, inputDir);
  // N-filtering may reduce count below numIters
  numIters = (int)params->size();
  auto load_end = std::chrono::system_clock::now();
  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
  std::vector<double> query_times_us(numIters);
#if (THREADING_ENABLED==1)
  #pragma omp parallel
  printf("launching thread %d\n",omp_get_thread_num());
  #pragma omp for
#endif
  for (int i=0; i < numIters; i++){ //loop over reads
    auto q_start = std::chrono::steady_clock::now();
    (*params)[i].score = gssw_soa_graph_fill(
        (*params)[i].graph, (*params)[i].prof);
    auto q_end = std::chrono::steady_clock::now();
    query_times_us[i] = std::chrono::duration<double, std::micro>(
        q_end - q_start).count();
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI
  gssw_print_timers();

  // Write scores (one per line)
  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  {
    std::ofstream scoreFile(std::string(OUT_DIR) + "/scores.txt");
    for (int i = 0; i < numIters; i++){
      scoreFile << (*params)[i].score << "\n";
    }
  }
  // Write per-query times
  {
    std::ofstream tFile(std::string(OUT_DIR) + "/query_times_us.txt");
    for (int i = 0; i < numIters; i++){
      tFile << query_times_us[i] << "\n";
    }
  }
  auto write_end = std::chrono::system_clock::now();

  // Timing breakdown
  auto load_time_us =
    std::chrono::duration_cast<std::chrono::microseconds>(
      load_end - load_start).count();
  auto kernel_time_us =
    std::chrono::duration_cast<std::chrono::microseconds>(
      kernel_end - kernel_start).count();
  auto write_time_us =
    std::chrono::duration_cast<std::chrono::microseconds>(
      write_end - write_start).count();
  std::cout << "load time: " << load_time_us << "us" << std::endl;
  std::cout << "kernel time: " << kernel_time_us << "us"
            << std::endl;
  std::cout << "write time: " << write_time_us << "us" << std::endl;

  // Save timing to file
  {
    std::ofstream timeFile(std::string(OUT_DIR) + "/time.txt");
    timeFile << "kernel_time_us: " << kernel_time_us << "\n";
    timeFile << "num_queries: " << numIters << "\n";
    timeFile << "avg_time_per_query_us: "
             << (double)kernel_time_us / numIters << "\n";
  }

  delete params;
}

