#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include "eval.h"
#include "gssw.h"
#include "nlohmann/json.hpp"
#include "gssw_to_json.hpp"

void write_ggm(std::string out_dir, int ind, gssw_graph_mapping* ggm){
  std::string stager("");
  stager = out_dir + "/mapping"+std::to_string(ind)+".json";
  FILE* out_file;
  out_file = fopen(stager.c_str(), "w");
  gssw_print_graph_mapping(ggm, out_file);
  fclose(out_file);
}

void init_output_dir(std::string output_dir){
  //delete old directory
  std::string stager("");
  stager = "rm -rf " + output_dir;
  std::system(stager.c_str());
  //make a new one
  stager = "mkdir " + output_dir;
  std::system(stager.c_str());
}
