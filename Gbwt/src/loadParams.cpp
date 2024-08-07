#include "loadParams.h"
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"


std::string searchStateToStr(gbwt::SearchState state){
  std::stringstream ss;
  ss << state.node << ": (" << std::get<0>(state.range) << "," << std::get<1>(state.range);
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

std::vector<std::vector<int>>* loadQueries(std::string inputDir, int numInputs){
  std::vector<std::vector<int>>* queries = 
      new std::vector<std::vector<int>>(numInputs);

  std::ifstream f(inputDir+"/Inputs/queries.txt");
  std::string line("");
  int i=0;
  while (std::getline(f, line)){
    std::istringstream lineStream(line);
    std::string nodeId("");
    while(std::getline(lineStream, nodeId, '>')){
      (*queries)[i].push_back(std::stoll(nodeId));
    }
  }
  return queries;
}

gbwt::GBWT* ldGbwt(std::string inputDir){
  std::ifstream f(inputDir+"/Inputs/g.gbwt");
  gbwt::GBWT* gbwt = new gbwt::GBWT();
  gbwt->load(f);
  return gbwt;
}

int ldNumInputs(std::string in_dir){
  std::ifstream f(in_dir+"/Inputs/queries.txt");
  //scan up until the appropriate line
  std::string line("");
  int i = 0;
  while (std::getline(f,line)){i++;}
  return i;
}
