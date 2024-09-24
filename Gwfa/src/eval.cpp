#include <fstream>
#include <eval.h>
#include <iostream>
#include <cassert>
#include <string>
#include "gfa-priv.h"

void writeResults(std::vector<gfa_edrst_t>* results, std::string outDir,
     int nIters){
  std::ofstream oFile(outDir + "/results.txt");
  for (int i = 0; i < nIters; i++){
    gfa_edrst_t r = (*results)[i];
    oFile << r.s << " ";
    //push the wlen variable which appears to be some kind of character count. I
    oFile << r.wlen << " ";

    oFile << "{ ";
    for(int i = 0; i < r.nv; i++){
      oFile << r.v[i] << " ";
    }
    oFile << "}";
    oFile << std::endl;
  }
}

void init_output_dir(std::string output_dir){
  int errorCode=0;
  //delete old directory
  std::string stager("");
  stager = "rm -rf " + output_dir;
  errorCode = std::max<int>(errorCode, std::system(stager.c_str()));
  //make a new one
  stager = "mkdir " + output_dir;
  errorCode = std::max<int>(errorCode, std::system(stager.c_str()));
  if (errorCode != 0){
    std::cerr << "Error building output directory!" << std::endl;
    assert(errorCode == 0);
  }
}
