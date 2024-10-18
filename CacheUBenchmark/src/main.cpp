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

//l1 dcache. 1.5MB
#define L1_SIZE (1.5*1048576)
//l2 cache. 40MB
#define L2_SIZE (40*1048576)
//l3 cache. 50MB
#define L3_SIZE (50*1048576)

#define ALL_CACHE_SIZE ((long)(L1_SIZE + L2_SIZE + L3_SIZE))

#define DATA_SIZE (ALL_CACHE_SIZE*10)

//#define N_PTR_CHASES 100000000
//#define N_PTR_CHASES 10000


#define SEED 42

void fillWithRands(long* arr, long size){
  for(long i=0; i<size; i++){
    arr[i] = std::rand();
  }
}

int main(int argc, char* argv[]){

  std::cout << "This warning always prints: Make sure you pass an integer argument " << std::endl;
  int nPtrChases = std::atoi(argv[1]);

  //This commendted out version is a copy of the store version
  std::srand(SEED);
  long loc = 0;
  long* data = new long[DATA_SIZE];
  fillWithRands(data, DATA_SIZE);

  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();

#if (THREADING_ENABLED==1)
  #pragma omp parallel 
  printf("launching thread %d\n",omp_get_thread_num());
  #pragma omp for
#endif
  for(long count=0; count < nPtrChases; count++){
    //we would just do data[loc], but that may have cycles, this way we are
    //really random
    loc = (data[loc] + loc + count) % DATA_SIZE;
  }
  printf("loc: %ld\n", loc); //just so we don't optimize out the whole loop
  auto kernel_end = std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI

  auto kernel_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
                                    kernel_end-kernel_start).count();
  std::cout << "kernel time: " << kernel_time_us << "us" << std::endl;
}

//You should expect L3 hit rate of approximately 1/10 once warm

  //This commendted out version is a copy of the store version
//  char* data = new char[ALL_CACHE_SIZE];
//  long* addrs = new long[ALL_CACHE_SIZE];
//  char* vals = new char[ALL_CACHE_SIZE];
//  for(long i=0; i<ALL_CACHE_SIZE; i++){
//    addrs[i] = std::rand()%ALL_CACHE_SIZE;
//  }
//  fillWithRands(vals, ALL_CACHE_SIZE);
//
//  //load stuff into data
//  for(int i=0; i<ALL_CACHE_SIZE; i++){
//    //This loop should have two cache hits, here and here, and then a bunch of
//    //misses
//    long addr = addrs[i];
//    char val = vals[i];
//    for (int j = 1; j < N_WRITES_PER_ADDR; j++){
//      //we're hashing here. We do val**4 * addr to fill up those long bits. 
//      //Multiply by
//      //j to shake things up each iteration. Finally, the xor does a real random
//      //hash between the addr and this somewhat random number
//      data[(addr^((long)j*addr*(long)val*(long)val*(long)val))%ALL_CACHE_SIZE]=val;
//    }
//  }
