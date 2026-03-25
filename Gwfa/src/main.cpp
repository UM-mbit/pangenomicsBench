#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <fstream>
#include <set>
#include <cctype>
#include <cstdlib>
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
namespace {

int parseIterIndex(const std::string& line) {
  size_t start = line.find_first_of("-0123456789");
  if (start == std::string::npos) {
    return -1;
  }
  size_t end = start + 1;
  while (end < line.size()
      && std::isdigit(
        static_cast<unsigned char>(line[end]))) {
    ++end;
  }
  return std::stoi(line.substr(start, end - start));
}

std::set<int> loadSlowIters(const std::string& path) {
  std::ifstream sf(path);
  if (!sf) {
    std::cerr << "Failed to open slow-iteration file: "
      << path << std::endl;
    std::exit(1);
  }

  std::set<int> slowIters;
  std::string line;
  while (std::getline(sf, line)) {
    if (line.empty()) {
      continue;
    }
    int idx = parseIterIndex(line);
    if (idx >= 0) {
      slowIters.insert(idx);
    }
  }
  return slowIters;
}

void destroyZs(std::vector<void*>* zBuff) {
  if (!zBuff) {
    return;
  }
  for (void* z : *zBuff) {
    gfa_ed_destroy(z);
  }
  delete zBuff;
}

void destroyKms(std::vector<void*>* kmBuff) {
  if (!kmBuff) {
    return;
  }
  for (void* km : *kmBuff) {
    km_destroy(km);
  }
  delete kmBuff;
}

} // namespace

/*---------------------------------------------------
 * Baseline timing mode
 *-------------------------------------------------*/
int main(int argc, char* argv[]){
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0]
      << " <input-dir> <slow-iter-file>"
      << std::endl;
    return 1;
  }

  std::string inputDir = argv[1];
  std::string slowItersPath = argv[2];

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
  if (!graph) {
    std::cerr << "Failed to read graph from "
      << inputDir << "/Inputs/graph.gfa"
      << std::endl;
    return 1;
  }

  std::vector<void*>* kmBuff =
    initKms(v0Buff->size());
  gfa_edopt_t* opt = initOpt();
  gfa_edseq_t* es = gfa_edseq_init(graph);
  std::vector<void*>* zBuff = constructZs(
    kmBuff, opt, graph, es,
    queryGapLenBuff, queryGapBuff,
    v0Buff, end0Buff);

  std::set<int> slowIters =
    loadSlowIters(slowItersPath);
  int64_t totalKernelUs = 0;

  for (int i = 0; i < (int)zBuff->size(); i++) {
    if (slowIters.count(i) == 0) {
      continue;
    }

    gfa_edrst_t result = {};
    gfa_edtiming_t timing = {};
    gfa_ed_step_timed((*zBuff)[i],
      (*v1Buff)[i], (*end1Buff)[i],
      GDP_MAX_ED, &result, &timing);

    totalKernelUs += timing.gwfa_us
      + timing.subgraph_us;
    std::cout << i << " "
      << timing.gwfa_us << " "
      << timing.subgraph_us << std::endl;
  }

  std::cout << "total_kernel_us "
    << totalKernelUs << std::endl;
  std::cout << "i gwfaTime subgraphBuildtime"
    << std::endl;

  destroyZs(zBuff);
  destroyKms(kmBuff);
  gfa_edseq_destroy(graph->n_seg, es);
  gfa_destroy(graph);
  delete opt;
  delete v0Buff;
  delete queryGapBuff;
  delete queryGapLenBuff;
  delete end1Buff;
  delete end0Buff;
  delete v1Buff;
  return 0;
}
#endif /* VERIFY_DUMP */
