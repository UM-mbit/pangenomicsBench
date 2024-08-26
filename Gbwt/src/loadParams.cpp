#include "loadParams.h"
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"

std::string parseArgs(int argc, char* argv[]){
  if (argc == 2){
    return argv[1];
  } else {
    if (argc == 1){
      std::cerr << "No command-line argument provided. ";
    } else if (argc > 2){
      std::cerr << "too many command-line arguments provided. ";
    }
    std::cerr << "Please specify one argument, the path to the input directory" << std::endl;
    assert(argc == 2);
  }
  return "";
}

std::vector<std::vector<gbwt::node_type>>* loadQueries(std::string inputDir, int numInputs){
  std::vector<std::vector<gbwt::node_type>>* queries = 
      new std::vector<std::vector<gbwt::node_type>>(numInputs);

  std::ifstream f(inputDir+"/Inputs/queries.txt");
  std::string line("");
  int i=0;
  while (std::getline(f, line)){
    std::istringstream lineStream(line);
    std::string nodeId("");
    while(std::getline(lineStream, nodeId, '>')){
      (*queries)[i].push_back(std::stoll(nodeId));
    }
    i++;
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
