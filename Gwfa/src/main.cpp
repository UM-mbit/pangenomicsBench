#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <fstream>
#include <set>
#include <omp.h>
#include <algorithm>
#include <climits>
#include "gfa.h"
#include "gfa-priv.h"
#include "kalloc.h"
#include "loadParams.h"
#include "eval.h"

#include "profilingUtils.h"

#ifdef VERIFY_DUMP
#include "loadGwfa.h"
extern "C" int gfa_ed_dbg;
#endif

#define OUT_DIR "Out"

#ifdef VERIFY_DUMP
/*---------------------------------------------------
 * Verify mode: load GwfaDump, call gwfa() directly
 *-------------------------------------------------*/
int main(int argc, char* argv[]){
  std::string dumpDir = "GwfaDump";
  if (argc >= 2) dumpDir = argv[1];
  int numItersLimit = INT_MAX;
  if (argc >= 3) numItersLimit = std::stoi(argv[2]);

  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  auto inputs = loadGwfaDump(dumpDir);
  auto load_end = std::chrono::system_clock::now();

  int n = std::min((int)inputs.size(),
	numItersLimit);

  FILE *sfp = fopen("scores.txt", "w");

  std::cout << "Running Kernel" << std::endl;
  auto kernel_start = std::chrono::system_clock::now();
  for (int i = 0; i < n; i++) {
    auto &inp = inputs[i];
    auto tS = std::chrono::system_clock::now();
    int score;
    if (!inp.sub) {
      score = -1;
    } else {
      score = gwfa(inp.ql, inp.q.c_str(),
        inp.startV, inp.startOff,
        inp.endV, inp.endOff,
        inp.sub, inp.s_term, gfa_ed_dbg);
    }
    auto tE = std::chrono::system_clock::now();
    auto us = std::chrono::duration_cast<
      std::chrono::microseconds>(tE - tS).count();
    fprintf(sfp, "%d\n", score);
    fflush(sfp);
    std::cout << "i: " << i << std::endl;
    std::cout << "iterTime: " << us << "us"
      << std::endl;
    std::cout << "ql: " << inp.ql << std::endl;
    std::cout << std::endl;
    freeGwfaIterInput(inp);
  }
  auto kernel_end = std::chrono::system_clock::now();
  fclose(sfp);
  std::cout << "Kernel Complete" << std::endl;

  std::cout << "Writing Outputs" << std::endl;
  auto write_start = std::chrono::system_clock::now();
  auto write_end = std::chrono::system_clock::now();

  auto load_us = std::chrono::duration_cast<
    std::chrono::microseconds>(
    load_end - load_start).count();
  auto kernel_us = std::chrono::duration_cast<
    std::chrono::microseconds>(
    kernel_end - kernel_start).count();
  auto write_us = std::chrono::duration_cast<
    std::chrono::microseconds>(
    write_end - write_start).count();
  std::cout << "load time: " << load_us
    << "us" << std::endl;
  std::cout << "kernel time: " << kernel_us
    << "us" << std::endl;
  std::cout << "write time: " << write_us
    << "us" << std::endl;
}

#else
/*---------------------------------------------------
 * Normal mode (+ optional dump via -DDUMP_GWFA)
 *-------------------------------------------------*/
int main(int argc, char* argv[]){
  std::string inputDir =
    getInputDirFromArgs(argc, argv);
  int num_iter_override =
    getNumItersFromArgs(argc, argv);

  init_output_dir(OUT_DIR);

  //load the inputs
  std::cout << "Loading Inputs" << std::endl;
  auto load_start = std::chrono::system_clock::now();
  std::vector<uint32_t>* v0Buff =
    loadScalarInput<uint32_t>(
      inputDir+"/Inputs/v0.txt");
  std::vector<std::string>* queryGapBuff =
    loadStrInput(inputDir+"/Inputs/queryGap.txt");
  std::vector<uint32_t>* queryGapLenBuff =
    loadScalarInput<uint32_t>(
      inputDir+"/Inputs/queryGapLen.txt");
  std::vector<int32_t>* end1Buff =
    loadScalarInput<int32_t>(
      inputDir+"/Inputs/end1.txt");
  std::vector<int32_t>* end0Buff =
    loadScalarInput<int32_t>(
      inputDir+"/Inputs/end0.txt");
  std::vector<uint32_t>* v1Buff =
    loadScalarInput<uint32_t>(
      inputDir+"/Inputs/v1.txt");
  gfa_t* graph = gfa_read(
    (inputDir+"/Inputs/graph.gfa").c_str());
  std::vector<void*>* kmBuff =
    initKms(v0Buff->size());
  gfa_edopt_t* opt = initOpt();
  gfa_edseq_t* es = gfa_edseq_init(graph);
  std::vector<void*>* zBuff = constructZs(
    kmBuff, opt, graph, es,
    queryGapLenBuff, queryGapBuff,
    v0Buff, end0Buff);
  std::vector<gfa_edrst_t> results =
    std::vector<gfa_edrst_t>(v0Buff->size());

  auto load_end = std::chrono::system_clock::now();

  int numIters = std::min(
    (int) zBuff->size(), num_iter_override);

  // Load slow iteration indices
  std::set<int> slowIters;
  {
    std::ifstream sf("slowerThan10k.txt");
    std::string line;
    while (std::getline(sf, line)) {
      int idx = std::stoi(line.substr(2));
      slowIters.insert(idx);
    }
  }

  BEGIN_ROI
  std::cout << "Running Kernel" << std::endl;
  auto kernel_start =
    std::chrono::system_clock::now();
#if (THREADING_ENABLED==1)
  #pragma omp parallel
  printf("launching thread %d\n",
    omp_get_thread_num());
  #pragma omp for
#endif
  for (int i = 0; i < numIters; i++) {
    if (slowIters.count(i) == 0) continue;
    auto iterStart =
      std::chrono::system_clock::now();
    gfa_ed_step((*zBuff)[i],
      (*v1Buff)[i], (*end1Buff)[i],
      GDP_MAX_ED, &(results[i]));
    auto iterEnd =
      std::chrono::system_clock::now();
    auto iterTime = std::chrono::duration_cast<
      std::chrono::microseconds>(
      iterEnd - iterStart).count();
    std::cout << "i: " << i << std::endl;
    std::cout << "iterTime: "
      << iterTime << "us" << std::endl;
    std::cout << "(v0, end0), (v1, end1): ("
      << (*v0Buff)[i] << ", "
      << (*end0Buff)[i] << "), ("
      << (*v1Buff)[i] << ", "
      << (*end1Buff)[i] << ")" << std::endl;
    std::cout << "queryGapLen: "
      << (*queryGapLenBuff)[i] << std::endl;
    std::cout << std::endl;
  }
  auto kernel_end =
    std::chrono::system_clock::now();
  std::cout << "Kernel Complete" << std::endl;
  END_ROI

  std::cout << "Writing Outputs" << std::endl;
  auto write_start =
    std::chrono::system_clock::now();
  writeResults(&results, OUT_DIR, numIters);
  auto write_end =
    std::chrono::system_clock::now();

  auto load_time_us =
    std::chrono::duration_cast<
    std::chrono::microseconds>(
    load_end - load_start).count();
  auto kernel_time_us =
    std::chrono::duration_cast<
    std::chrono::microseconds>(
    kernel_end - kernel_start).count();
  auto write_time_us =
    std::chrono::duration_cast<
    std::chrono::microseconds>(
    write_end - write_start).count();
  std::cout << "load time: "
    << load_time_us << "us" << std::endl;
  std::cout << "kernel time: "
    << kernel_time_us << "us" << std::endl;
  std::cout << "write time: "
    << write_time_us << "us" << std::endl;

#ifdef DUMP_GWFA
  dump_gwfa_flush();
#endif
}
#endif /* VERIFY_DUMP */
