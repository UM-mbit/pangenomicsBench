#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include "eval.h"
#include "loadParams.h"
#include "nlohmann/json.hpp"

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

void writeTraces(std::vector<std::vector<OnewayTrace>>& readTraces, 
                 std::ostream& file){
  for (int i = 0; i < readTraces.size(); i++){
    nJson readJson;
    std::vector<OnewayTrace>& traces = readTraces[i];
    readJson["index"] = i;
    readJson["traces"] = nJson::array();
    for(OnewayTrace& t : traces){
      readJson["traces"].push_back(t.toJson());
    }
    file << readJson.dump(2) << std::endl;
  }
}
