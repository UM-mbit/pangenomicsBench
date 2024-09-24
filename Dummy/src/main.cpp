#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cstring>
#include <vector>
#include <string>
#include <cassert>
#include <omp.h>
#include <chrono>

#include "profilingUtils.h"

#define N_ITERS 1000000


/* this is pretrty much the kernel. It's just used for doing some dumb work to
 * take time
 */
int doWork(){
  int x=0;
  for (int i=0; i < 1000; i++){
    int y = std::rand();
    if (y%2 == 0){
      x += std::rand();
    }
  }
  return x;
}

void writeOutput(int val){
  std::string output_dir = "Out";
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
  std::ofstream oFile = std::ofstream(output_dir+"/sum.txt");
  oFile << val << std::endl;
}

int loadInput(std::string inputDir){
    std::ifstream f(inputDir+"/seed.txt");
    std::string line("");
    std::getline(f, line);
    return std::stoll(line);
}

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

int main(int argc, char* argv[]){
  std::string inputDir = parseArgs(argc, argv);

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  int seed = loadInput(inputDir+"/Input");
  int x=0;
  doWork();
  auto load_end = std::chrono::system_clock::now();

  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
  std::srand(seed);
#if (THREADING_ENABLED==1)
  #pragma omp parallel 
  printf("launching thread %d\n",omp_get_thread_num());
  #pragma omp for
#endif
  for (int i=0; i < N_ITERS; i++){
    //allocate a huge amount of memory to get some interesting cache behaviour
    int BIGARRSIZE=10000000;
    int* bigArr = new int[BIGARRSIZE];
    int y = std::rand();
    if (y%2 == 0){
      bigArr[y%BIGARRSIZE] = std::rand();
      y = bigArr[std::rand()/BIGARRSIZE];
      x += y;
      y = std::rand();
    }
    delete bigArr;
  }
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI

  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  writeOutput(x);
  auto write_end = std::chrono::system_clock::now();

  //write the timing breakdown
  auto load_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    load_end-load_start).count();
  auto kernel_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    kernel_end-kernel_start).count();
  auto write_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    write_end-write_start).count();
  std::cout << "load time: " << load_time_us << "us" << std::endl;
  std::cout << "kernel time: " << kernel_time_us << "us" << std::endl;
  std::cout << "write time: " << write_time_us << "us" << std::endl;
}

