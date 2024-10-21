#include <iostream>
#include <cstring>
#include <fstream>
#include <stdint.h>
#include <cstdlib>
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
  std::vector<ReadAlignmentParams>* params =
      load_read_alignment_params(num_inputs, inputDir);
  auto load_end = std::chrono::system_clock::now();
  
  int numIters = std::min(num_inputs, num_iter_override);
  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
#if (THREADING_ENABLED==1)
  #pragma omp parallel for 
  for (int i=0; i < numIters; i++){ //loop over reads (or really anchors)
#else
  for (int i=0; i < numIters; i++){ //loop over reads (or really anchors)
#endif
    //load inputs
    gssw_graph* graph = (*params)[i].graph;
    std::string seq = (*params)[i].seq;
    int8_t* nt_table = (*params)[i].nt_table;
    int8_t* score_matrix = (*params)[i].score_matrix;

    //run the kernel
    gssw_graph_fill_pinned( graph,
                            seq.c_str(),
                            nt_table,
                            score_matrix,
                            constants::weight_gapO, 
                            constants::weight_gapE, 
                            constants::start_full_length_bonus,
                            constants::end_full_length_bonus,
                            constants::maskLen,
                            constants::score_size,
                            constants::save_matrixes );

  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI

  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  for (int i = 0; i < numIters; i++){
    //load inputs
    gssw_graph* graph = (*params)[i].graph;
    std::string seq = (*params)[i].seq;
    int8_t* nt_table = (*params)[i].nt_table;
    int8_t* score_matrix = (*params)[i].score_matrix;

    //generate outputs
    gssw_graph_mapping* ggm = gssw_graph_trace_back( 
                                   graph,
                                   seq.c_str(),
                                   seq.size(),
                                   nt_table,
                                   score_matrix,
                                   constants::weight_gapO, 
                                   constants::weight_gapE, 
                                   constants::start_full_length_bonus,
                                   constants::end_full_length_bonus);
    //write outputs
    write_ggm(OUT_DIR, ggm);
  
    //cleanup
    gssw_graph_mapping_destroy(ggm);
  }
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

  
  delete params;
}

