#include "loadParams.h"
#include <cassert>
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include "gssw_to_json.hpp"

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

ReadAlignmentParams::~ReadAlignmentParams(){
  gssw_graph_destroy(graph);
  delete nt_table;
  delete score_matrix;
}

std::vector<ReadAlignmentParams>* load_read_alignment_params(size_t num_inputs,
                                                        std::string input_dir){
  std::vector<ReadAlignmentParams>* params = 
         new std::vector<ReadAlignmentParams>(num_inputs);

  for (int i = 0; i < num_inputs; i++){
    (*params)[i].graph = ld_gssw_graph(input_dir, i);
    std::string seq = ld_seq(input_dir, i);
    (*params)[i].seq = seq;
    (*params)[i].nt_table = get_nt_table(seq.size());
    (*params)[i].score_matrix = get_score_matrix();
  }
  return params;
}

int8_t* get_nt_table(size_t seqLen){
//This one is a little funky. In vg it is initialized according to a constant
//pattern that uses some for loops. Here we've just taken the first 255 elements
//and will use substrings to generate however many are needed
  static int8_t full_nt_table[255]{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 71, 67, 84, 67, 0, -15, 32, -96, -94, 127, 0, 0, -128, -53, 32, -96, -94, 127, 0, 0, -128, -105, 33, -96, -94, 127, 0, 0, 48, -1, -122, -96, -94, 127, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 83, 50, 50, 53, 95, 52, 56, 50, 49, 52, 57, 0, -2, 127, 0, 0, 80, -1, -122, -96, -94, 127, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 83, 50, 50, 53, 95, 52, 56, 50, 49, 52, 57, 0, 71, 84, 67, 71, -92, 0, 0, 0, 65, 71, 65, 67, 13, 0, 0, 0, 0, 0, 0, 0, -92, 0, 0, 0, -91, 0, 0, 0, 0, 4, 4, 4, 4, 4};
  int8_t* nt_table = new int8_t[255];
  for (int i = 0; i < seqLen; i++){
    nt_table[i] = full_nt_table[i];
  }
  return nt_table;
}

int8_t* get_score_matrix(){
  int8_t* score_matrix = new int8_t[25]{1,-4,-4,-4, 0,
                                       -4, 1,-4,-4, 0,
                                       -4,-4, 1,-4, 0,
                                       -4,-4,-4, 1, 0,
                                        0, 0, 0, 0, 0};
  return score_matrix;
}

gssw_graph* ld_gssw_graph(std::string in_dir, int ind){
  //std::cerr << "about to open the file" << std::endl;
  std::ifstream f(in_dir+"/Inputs/Graphs/g"+std::to_string(ind)+".json");
  //std::cerr << in_dir+"/Inputs/Graphs/g"+std::to_string(ind)+".json" << std::endl;
  if (!f.is_open()) {
              throw std::runtime_error("Could not open file");
  }
  nlohmann::json data = nlohmann::json::parse(f);
  //std::cerr << data << std::endl;
  gssw_graph* graph = ld_graph(data);
  //gssw_graph_print(graph);
  return graph;
}


std::string ld_seq(std::string in_dir, int ind){
  std::string data("");
  std::string lineNum("");
  std::ifstream f(in_dir+"/Inputs/reads.txt");

  //scan up until the appropriate line
  std::string line("");
  std::getline(f,line);
  int i = 0;
  while (i < ind) { std::getline(f,line); i++;}
  std::istringstream lineStream(line);
  
  //strip the number from line
  std::getline(lineStream, lineNum, ' ');
  //get the remainder (the data)
  std::getline(lineStream, data);

  return data;
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
