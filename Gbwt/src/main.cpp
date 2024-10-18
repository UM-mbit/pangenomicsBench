#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <omp.h>

#include "loadParams.h"
#include "eval.h"
#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"
#include "profilingUtils.h"

#include <cassert>


#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(int argc, char* argv[]){
  std::string inputDir = getInputDirFromArgs(argc, argv);
  int numIterOverride = getNumItersFromArgs(argc, argv);

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  init_output_dir(OUT_DIR);
  int numInputs = ldNumInputs(inputDir);
  int numIters = std::min(numIterOverride, numInputs);
  std::vector<std::vector<gbwt::node_type>>* queries = loadQueries(inputDir,numInputs);
  std::vector<gbwt::SearchState> queryResults = 
      std::vector<gbwt::SearchState>(numIters);
  gbwt::GBWT* gbwtIndex = ldGbwt(inputDir);
  auto load_end = std::chrono::system_clock::now();

  
  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
#if (THREADING_ENABLED==1)
  #pragma omp parallel for
  for (int i=0; i < numIters; i++){ //loop over queries
#else
  for (int i=0; i < numIters; i++){ //loop over queries
#endif
    std::vector<gbwt::node_type>& query = (*queries)[i];
    queryResults[i] = gbwtIndex->find(query.begin(), query.end());
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI
  
  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  dumpStatesToFile(OUT_DIR, queryResults);
  auto write_end = std::chrono::system_clock::now();
  
  //write the timing breakdown
  auto load_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    load_end-load_start).count();
  auto kernel_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    kernel_end-kernel_start).count();
  auto write_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    write_end-write_start).count();
  std::cout << "load time: " << load_time_us << "us" << std::endl;
  std::cout << "kernel time: " << kernel_time_us << "us" << std::endl;
  std::cout << "write time: " << write_time_us << "us" << std::endl;

  
  delete queries;
}

