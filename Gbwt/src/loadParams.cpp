#include "loadParams.h"
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <limits.h>

#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"

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
  //Now that you've loaded everything, encode the nodes
  for (std::vector<gbwt::node_type>& query : *queries){
    for (gbwt::node_type& n : query){
      n = gbwt::Node::encode(n, false);
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
