#include "loadParams.h"
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

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

int ld_num_inputs(std::string in_dir){
  std::string data("");
  std::string lineNum("");
  std::ifstream f(in_dir+"/Inputs/reads.txt");

  //scan up until the appropriate line
  std::string line("");
  int i = 0;
  while (std::getline(f,line)){i++;}
  return i;
}
