#include <iostream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <omp.h>

#include "loadParams.h"
#include "eval.h"
#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"
#include "vtuneConfigs.h"

#include <cassert>


#define OUT_DIR "Out" //NOTE, be responsible. rm -rf OUT_DIR is called

int main(int argc, char* argv[]){
  std::string inputDir = parseArgs(argc, argv);

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  init_output_dir(OUT_DIR);
  int numInputs = ldNumInputs(inputDir);
  std::vector<std::vector<gbwt::node_type>>* queries = loadQueries(inputDir,numInputs);
  std::vector<gbwt::SearchState> queryResults = 
      std::vector<gbwt::SearchState>(numInputs);
  gbwt::GBWT* gbwtIndex = ldGbwt(inputDir);
  auto load_end = std::chrono::system_clock::now();
  
  //debug
  //for (int i = 0; i < 548; i++){
  //  std::vector<gbwt::short_type> v = gbwtIndex->extract(0);
  //  for (gbwt::short_type n : v){
  //    //This is always 1, which seems wrong
  //    std::cerr << "size of the edge" << gbwtIndex->record(n).outdegree() << std::endl;
  //    //for (gbwt::edge_type e : gbwtIndex->record(n).outgoing){
  //    //  std::cerr << "(" << std::get<0>(e) << ", "<< std::get<1>(e) << "), ";
  //    //}
  //  }
  //  std::cerr << std::endl;
  //}
  //std::vector<gbwt::node_type> q = {4242, 4244, 4246, 4248, 4250, 4252, 4254, 4256, 4258,};
  std::vector<gbwt::node_type> q{103,104,105,106,107,108,109,110};
  for (gbwt::node_type& n : q ){
    n = gbwt::Node::encode(n, false);
    std::cout << n << ", " <<std::endl;
  }
  //reversed
  //std::cerr << "manicured " << gbwtIndex->prefix(q.rbegin(), q.rend());
  std::cerr << "manicured " << gbwtIndex->find(q.begin(), q.end());
  
  std::cerr << std::endl;
  //assert(false);
  VTUNE_BEGIN
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
#if (OMP_ENABLED==1)
  #pragma omp parallel for
#endif
  for (int i=0; i < numInputs; i++){ //loop over queries
    std::vector<gbwt::node_type>& query = (*queries)[i];
    //for (int j = 0; j < query.size(); j++){
      //std::cerr << query[j] << "<";
      //gbwt::node_type node = query[j];
      //assert ((index).contains(static_cast<gbwt::node_type>(node))); //the node should always be in graph
    //}
    //std::vector<int> doubleTime{};
    //for (int j=0; j < query.size(); j+=2){
    //  doubleTime.push_back(query[j]);
    //}
    //query = doubleTime;
    //std::cerr << std::endl;
    for (gbwt::node_type& n : query){
      n = gbwt::Node::encode(n, false);
      //std::cerr << "zkn" << gbwtIndex->contains(n*2) << "node" << n << std::endl;
    }
    queryResults[i] = gbwtIndex->find(query.begin(), query.end());
    //std::cerr << queryResults[i].size() << std::endl;
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  VTUNE_END
  
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

