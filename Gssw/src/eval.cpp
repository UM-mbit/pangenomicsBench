#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>
#include "eval.h"
#include "gssw.h"
#include "nlohmann/json.hpp"
#include "gssw_to_json.hpp"

void write_ggm(std::string out_dir, gssw_graph_mapping* ggm){
  std::string stager("");
  stager = out_dir + "/mapping.json";
  FILE* out_file;
  out_file = fopen(stager.c_str(), "a");
  gssw_print_graph_mapping(ggm, out_file);
  fclose(out_file);
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
