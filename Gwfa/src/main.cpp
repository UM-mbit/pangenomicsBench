#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <omp.h>
#include <algorithm>
#include "gfa.h"
#include "gfa-priv.h"
#include "kalloc.h"
#include "loadParams.h"
#include "eval.h"

#include "profilingUtils.h"

#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(int argc, char* argv[]){
  std::string inputDir = getInputDirFromArgs(argc, argv);
  int num_iter_override = getNumItersFromArgs(argc, argv);

  init_output_dir(OUT_DIR);

  //load the inputs
  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  std::vector<uint32_t>*    v0Buff=loadScalarInput<uint32_t>(
                                   inputDir+"/Inputs/v0.txt");
  std::vector<std::string>* queryGapBuff=loadStrInput(
                                         inputDir+"/Inputs/queryGap.txt");
  std::vector<uint32_t>*    queryGapLenBuff=loadScalarInput<uint32_t>(
                                     inputDir+"/Inputs/queryGapLen.txt");
  std::vector<int32_t>*     end1Buff=loadScalarInput<int32_t>(
                                     inputDir+"/Inputs/end1.txt");
  std::vector<int32_t>*     end0Buff=loadScalarInput<int32_t>(
                                     inputDir+"/Inputs/end0.txt");
  std::vector<uint32_t>*    v1Buff=loadScalarInput<uint32_t>(
                                   inputDir+"/Inputs/v1.txt");
  gfa_t*                    graph=gfa_read(
                                  (inputDir+"/Inputs/graph.gfa").c_str());
  std::vector<void*>*       kmBuff=initKms(v0Buff->size());
  gfa_edopt_t*              opt=initOpt();
  gfa_edseq_t*              es=gfa_edseq_init(graph);
  std::vector<void*>*       zBuff=constructZs(kmBuff, opt, graph, es,
                               queryGapLenBuff, queryGapBuff, v0Buff, end0Buff);
  std::vector<gfa_edrst_t> results=std::vector<gfa_edrst_t>(v0Buff->size());

  auto load_end = std::chrono::system_clock::now();

  int numIters = std::min((int) zBuff->size(), num_iter_override);

  //DEBUG
  //gfa_print(graph, stdout, 0);
  
  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
#if (THREADING_ENABLED==1)
  #pragma omp parallel for
  for (int i=0; i < numIters; i++){ //loop over reads (or really anchors)
#else
  for (int i=0; i < numIters; i++){ //loop over reads (or really anchors)
#endif
    auto iterStart = std::chrono::system_clock::now();
    //run the kernel
    gfa_ed_step((*zBuff)[i],
                (*v1Buff)[i],
                (*end1Buff)[i],
                GDP_MAX_ED,
                &(results[i]));
    auto iterEnd = std::chrono::system_clock::now();
    auto iterTime = std::chrono::duration_cast<std::chrono::microseconds>(
                                    iterEnd-iterStart).count();
    //std::cout << "i: " << i << std::endl;
    //std::cout << "iterTime: " << iterTime << "us" << std::endl;
    //std::cout << "(v0, end0), (v1, end1): (" << (*v0Buff)[i] << ", " << (*end0Buff)[i] << "), (" << (*v1Buff)[i] << ", " << (*end1Buff)[i] << ")" << std::endl;
    //std::cout << "queryGapLen: " << (*queryGapLenBuff)[i] << std::endl;
    //std::cout << std::endl;
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI

  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  writeResults(&results, OUT_DIR, numIters);
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
}


