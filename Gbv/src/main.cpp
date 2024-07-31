#include <iostream>
#include <cstring>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
#include <vector>
#include <chrono>

#include "loadParams.h"
#include "eval.h"
#include "nlohmann/json.hpp"

#define INPUT_DIR "/data2/kaplannp/Genomics/Datasets/Kernels/Gbv"
//#define INPUT_DIR "/data2/kaplannp/Genomics/DumpTrue"
//#define INPUT_DIR "/data2/kaplannp/Genomics/Dump"
#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

    //  const DPSlice& initialSlice,
		//auto initialSlice = BV::getInitialEmptySlice();
int main(){

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();

  init_output_dir(OUT_DIR);
  int numInputs = ld_num_inputs(INPUT_DIR);
  std::vector<std::vector<int64_t>>* maxScoresVec = 
      getSliceMaxScores(INPUT_DIR, numInputs);
  std::pair<Params*, SerializableParams*> paramPair = loadParams(INPUT_DIR);
  Params* params = std::get<0>(paramPair);
  std::vector<std::string>* seqs = loadSequences(INPUT_DIR);
  std::vector<std::string>* revSeqs = getRevSequence(seqs);
  std::vector<std::vector<ProcessedSeedHit>>* seedHitVecs = 
      loadSeedHits(INPUT_DIR, numInputs);
  ReusableState* reusableState = getReusableState(params);
	BitvectorAligner bvAligner(*params);
  //for holding the outputs
  std::vector<std::vector<OnewayTrace>> outputTraces(numInputs);
  auto load_end = std::chrono::system_clock::now();

  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();

  //For loop over reads.
  for (int i=0; i < numInputs; i++){
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

  //write outputs for comparison
  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();

  //dump traces to file
  
  std::string stager = std::string(OUT_DIR) + std::string("/traces.json");
  std::ofstream traceOutFile(stager);
  writeTraces(outputTraces, traceOutFile);

  auto write_end = std::chrono::system_clock::now();

  //cleanup
  delete std::get<0>(paramPair); //params
  delete std::get<1>(paramPair); //the SerializableParams
  delete seqs;
  delete revSeqs;
  delete seedHitVecs;
  delete maxScoresVec;
  delete reusableState;
}
