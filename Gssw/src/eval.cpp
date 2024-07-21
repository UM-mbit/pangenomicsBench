#include <string>
#include <cstdlib>
#include <fstream>
#include "eval.h"
#include "gssw.h"
#include "nlohmann/json.hpp"
#include "gssw_to_json.hpp"

void write_ggm(std::string out_dir, int ind, gssw_graph_mapping* ggm){
  std::ofstream out_file(out_dir + "/mapping"+std::to_string(ind)+".json");
  //write outputs to file
  nlohmann::json ggm_json_serialized = dump_graph_mapping(ggm);
  std::string ggm_json_string = ggm_json_serialized.dump(2);
  out_file << ggm_json_string << std::endl;
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
