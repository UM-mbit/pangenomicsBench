#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <omp.h>
#include "gfa.h"
#include "gfa-priv.h"
#include "kalloc.h"
#include "loadParams.h"
#include "eval.h"

#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(int argc, char* argv[]){
  std::string inputDir = parseArgs(argc, argv);

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

  //DEBUG
  //gfa_print(graph, stdout, 0);
  
  //BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
#if (THREADING_ENABLED==1)
  #pragma omp parallel for
#endif
  for (int i=0; i < zBuff->size(); i++){ //loop over reads (or really anchors)
                                        //Note zbuff is same size as others
    //run the kernel
    gfa_ed_step((*zBuff)[i],
                (*v1Buff)[i],
                (*end1Buff)[i],
                GDP_MAX_ED,
                &(results[i]));
    //std::cerr << (*zBuff)[i],
    //std::cerr << "v1 " << (*v1Buff)[i] << std::endl;
    //std::cerr << "end1 " << (*end1Buff)[i] << std::endl;
    //std::cerr << "max " << GDP_MAX_ED << std::endl;
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  //END_ROI

  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  writeResults(&results, OUT_DIR);
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


