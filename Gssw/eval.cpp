#include "eval.h"
#include "deps/Gssw/src/gssw.h"
#include <string>
#include "deps/vg/deps/nlohmann-json/single_include/nlohmann/json.hpp"
#include "deps/vg/src/gssw_to_json.hpp"

void write_ggm(std::ofstream& out_file, gssw_graph_mapping* ggm){
  std::ofstream
  //write outputs to file
  nlohmann::json ggm_json_serialized = dump_graph_mapping(ggm);
  std::string ggm_json_string = ggm_json_serialized.dump(2);
  out_file << ggm_json_string << std::endl;
  gssw_graph_mapping_destroy(ggm);
}
