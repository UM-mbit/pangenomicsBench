#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <limits.h>

#include "loadParams.h"
#include "gfa.h"
#include "gfa-priv.h"
#include "kalloc.h"
#include "gfa.h"

std::string getInputDirFromArgs(int argc, char* argv[]){
  if (argc == 2 || argc == 3){ //either no n_iters or n_iters provided
    return argv[1]; //return the input directory
  } else { //too many or to few args
    if (argc == 1){
      std::cerr << "No command-line argument provided. ";
    } else if (argc > 3){
      std::cerr << "too many command-line arguments provided. ";
    }
    std::cerr << "Please specify one argument, the path to the input directory, and an optional second argument, the number of iterations" << std::endl;
    assert(false);
  }
  return ""; //should never reach this point
}

int getNumItersFromArgs(int argc, char* argv[]){
  if (argc == 2){
    return INT_MAX; //default if no number of iterations is provided
  } else {
    if (argc == 3){
      return std::stoi(argv[2]); //means n_iters was provided
    } 
    else { //either too few or too many args
      if (argc == 1){
        std::cerr << "No command-line argument provided. ";
      } else if (argc > 3){
        std::cerr << "too many command-line arguments provided. ";
      }
      std::cerr << "Please specify one argument, the path to the input directory, and an optional second argument, the number of iterations" << std::endl;
      assert(false);
    }
  }
  return 0; //should never reach this point
}

std::vector<std::string>* loadStrInput(std::string filepath){
  std::vector<std::string>* inputs = new std::vector<std::string>();
  std::string line("");
  std::ifstream f(filepath);
  while (std::getline(f, line)){
    inputs->push_back(line);
  }
  return inputs;
}

const gfa_edseq_t* loadEs(gfa_t* g){
  return gfa_edseq_init(g);
}

std::vector<void*>* initKms(uint32_t size){
  std::vector<void*>* kms = new std::vector<void*>();
  for (uint32_t i = 0; i < size; i++){
    kms->push_back(km_init());
  }
  return kms;
}

std::vector<void*>* constructZs( std::vector<void*>* kmBuff,
                                 gfa_edopt_t* opt,
                                 gfa_t* graph,
                                 gfa_edseq_t* es,
                                 std::vector<uint32_t>* queryGapLenBuff,
                                 std::vector<std::string>* queryGapBuff,
                                 std::vector<uint32_t>* v0Buff,
                                 std::vector<int32_t>* end0Buff){

  std::vector<void*>* zBuff = new std::vector<void*>();

  //note that v0Buff->size() is the same for all of these vectors
  for(uint32_t i = 0; i < v0Buff->size(); i++){
    void* z = gfa_ed_init(
        (*kmBuff)[i],
        opt,
        graph,
        es,
        (*queryGapLenBuff)[i],
        (*queryGapBuff)[i].c_str(),
        (*v0Buff)[i],
        (*end0Buff)[i]
        );
    zBuff->push_back(z);
  }
  return zBuff;
}

gfa_edopt_t* initOpt(){
  gfa_edopt_t* opt = new gfa_edopt_t(); 
  gfa_edopt_init(opt); //allocates some space and sets (bad) default values 
  //These hardcoded values are the same way they are assigned in minigraph code
  //that calls the kernel
	opt->traceback = 1;
  opt->max_chk = 1000;
  opt->bw_dyn = 1000;
  opt->max_lag = GDP_MAX_ED/2;
	opt->i_term = 500000000LL;
  return opt;
}
