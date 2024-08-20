#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>
#include "eval.h"

#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"

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

std::string searchStateToStr(gbwt::SearchState state){
  std::stringstream ss;
  ss << state.node << ": (" << std::get<0>(state.range) << "," << std::get<1>(state.range) << ")";
  return ss.str();
}

void dumpStatesToFile(std::string outDir, 
    std::vector<gbwt::SearchState> queryOuts){
  std::ofstream f(outDir+"/queryOuts.txt");
  
  //output a header
  f << "node: (startRange,endRange)" << std::endl;
  
  //dump each state to a new line
  for(gbwt::SearchState s : queryOuts){
    f << searchStateToStr(s) << std::endl;
  }
}
