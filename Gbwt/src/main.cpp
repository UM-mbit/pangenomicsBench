#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>

#include "loadParams.h"
#include "eval.h"
#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"


#define INPUT_DIR "/data2/kaplannp/Genomics/Datasets/Kernels/Gbwt"
#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(){

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  init_output_dir(OUT_DIR);
  int numInputs = ldNumInputs(INPUT_DIR);
  std::vector<std::vector<int>>* queries = loadQueries(INPUT_DIR,numInputs);
  std::vector<gbwt::SearchState> queryResults = 
      std::vector<gbwt::SearchState>(numInputs);
  gbwt::GBWT* gbwtIndex = ldGbwt(INPUT_DIR);
  auto load_end = std::chrono::system_clock::now();
  
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
  //For loop over reads.
  for (int i=0; i < numInputs; i++){
    std::vector<int>& query = (*queries)[i];
    gbwtIndex->prefix(query.begin(), query.end());
  }
  std::cout << "Kernel Complete" << std::endl;
  auto kernel_end = std::chrono::system_clock::now();
  
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

