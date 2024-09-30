#include <iostream>
#include <cstring>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <omp.h>

#include "loadParams.h"
#include "eval.h"
#include "nlohmann/json.hpp"
#include "profilingUtils.h"

#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(int argc, char* argv[]){
  std::string inputDir = getInputDirFromArgs(argc, argv);
  int num_iter_override = getNumItersFromArgs(argc, argv);

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();

  init_output_dir(OUT_DIR);
  int numInputs = ld_num_inputs(inputDir);
  int numIters = std::min(num_iter_override, numInputs);
  std::vector<std::vector<int64_t>>* maxScoresVec = 
      getSliceMaxScores(inputDir, numInputs);
  std::pair<Params*, SerializableParams*> paramPair = loadParams(inputDir);
  Params* params = std::get<0>(paramPair);
  std::vector<std::string>* seqs = loadSequences(inputDir);
  std::vector<std::string>* revSeqs = getRevSequence(seqs);
  std::vector<std::vector<ProcessedSeedHit>>* seedHitVecs = 
      loadSeedHits(inputDir, numInputs);
  ReusableState* reusableState = getReusableState(params);
	BitvectorAligner bvAligner(*params);
  //for holding the outputs
  std::vector<std::vector<OnewayTrace>> outputTraces(numIters);
  auto load_end = std::chrono::system_clock::now();

  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
#if (THREADING_ENABLED==1)
  #pragma omp parallel 
  printf("launching thread %d\n",omp_get_thread_num());
  #pragma omp for
#endif
  for (int i=0; i < numIters; i++){ //loop over reads (or really anchors)
    //load inputs
    std::string& seq = (*seqs)[i];
    std::string& revSeq = (*revSeqs)[i];
    std::vector<ProcessedSeedHit>& seedHits = (*seedHitVecs)[i];
    std::vector<int64_t>& sliceMaxScores = (*maxScoresVec)[i];
    reusableState->clear();

	  outputTraces[i] = bvAligner.getMultiseedTraces( seq, 
                                          revSeq, 
                                          seedHits, 
                                          *reusableState, 
                                          sliceMaxScores);
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI

  //write outputs for comparison
  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();

  //dump traces to file
  
  std::string stager = std::string(OUT_DIR) + std::string("/traces.json");
  std::ofstream traceOutFile(stager);
  writeTraces(outputTraces, traceOutFile);

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

  //cleanup
  delete std::get<0>(paramPair); //params
  delete std::get<1>(paramPair); //the SerializableParams
  delete seqs;
  delete revSeqs;
  delete seedHitVecs;
  delete maxScoresVec;
  delete reusableState;
}
