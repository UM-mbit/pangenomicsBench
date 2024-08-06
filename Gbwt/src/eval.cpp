#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include "eval.h"

void init_output_dir(std::string output_dir){
  //delete old directory
  std::string stager("");
  stager = "rm -rf " + output_dir;
  std::system(stager.c_str());
  //make a new one
  stager = "mkdir " + output_dir;
  std::system(stager.c_str());
}
