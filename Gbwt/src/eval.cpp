#include <string>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "eval.h"

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
