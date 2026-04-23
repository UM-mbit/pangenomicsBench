#include "loadParams.h"
#include <cassert>
#include <stdint.h>
#include <cstdlib>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <limits.h>
#include <algorithm>

std::string getInputDirFromArgs(int argc, char* argv[]){
  if (argc == 2 || argc == 3){ //either no n_iters or n_iters provided
    return argv[1]; //return the input directory
  } else { //too many or to few args
    if (argc == 1){
      std::cerr << "No command-line argument provided. ";
    } else if (argc > 3){
      std::cerr << "too many command-line arguments provided. ";
    }
    std::cerr << "Please specify one argument, the path to the input directory, and an optional second argument, the number of iterations" << std::endl;
    assert(false);
  }
  return ""; //should never reach this point
}

int getNumItersFromArgs(int argc, char* argv[]){
  if (argc == 2){
    return INT_MAX; //default if no number of iterations is provided
  } else {
    if (argc == 3){
      return std::stoi(argv[2]); //means n_iters was provided
    } 
    else { //either too few or too many args
      if (argc == 1){
        std::cerr << "No command-line argument provided. ";
      } else if (argc > 3){
        std::cerr << "too many command-line arguments provided. ";
      }
      std::cerr << "Please specify one argument, the path to the input directory, and an optional second argument, the number of iterations" << std::endl;
      assert(false);
    }
  }
  return 0; //should never reach this point
}

ReadAlignmentParams::~ReadAlignmentParams(){
  if (graph) gssw_soa_graph_destroy(graph);
  if (prof) gssw_init_destroy(prof);
}

// Check if a read sequence contains N or n
static bool read_has_n(const std::string& seq){
  for (char c : seq){
    if (c == 'N' || c == 'n') return true;
  }
  return false;
}

// Check if any byte in the SoA graph's seq array is N (value 4)
static bool soa_graph_has_n(gssw_soa_graph* g){
  for (uint32_t i = 0; i < g->total_seq; i++){
    if (g->seqs[i] == 4) return true;
  }
  return false;
}

// Check if all required text files exist
static bool text_data_exists(const std::string& input_dir){
  std::ifstream g(input_dir + "/Inputs/graph.soa");
  std::ifstream p(input_dir + "/Inputs/matchProfiles.txt");
  return g.good() && p.good();
}

// Load a single precomputed profile from text file stream
static gssw_profile* load_profile_text(std::ifstream& f){
  int32_t readLen;
  f >> readLen;
  f.ignore();

  int32_t segLen = (readLen + 15) / 16;
  int32_t n = 4;
  gssw_profile* p = (gssw_profile*)calloc(
      1, sizeof(struct gssw_profile));
  p->readLen = readLen;
  p->bias = 4;
  p->profile_byte = (__m128i*)malloc(
      n * segLen * sizeof(__m128i));

  uint8_t* bytes = (uint8_t*)p->profile_byte;
  std::string line;
  for (int32_t base = 0; base < n; base++){
    std::getline(f, line);
    std::istringstream ss(line);
    for (int32_t j = 0; j < segLen * 16; j++){
      int v; ss >> v;
      bytes[base * segLen * 16 + j] = (uint8_t)v;
    }
  }
  return p;
}

// Load a single SoA graph from an open text file stream
static gssw_soa_graph* load_graph_text(std::ifstream& f){
  gssw_soa_graph* g =
      (gssw_soa_graph*)calloc(1, sizeof(gssw_soa_graph));
  f >> g->num_nodes >> g->total_nexts >> g->total_seq;
  f.ignore(); // skip newline

  // Parse node descriptors: (seq_off,seq_len,next_off,next_len)
  g->nodes = (gssw_node_desc*)malloc(
      g->num_nodes * sizeof(gssw_node_desc));
  std::string line;
  std::getline(f, line);
  {
    std::istringstream ss(line);
    for (uint32_t i = 0; i < g->num_nodes; i++){
      char paren, comma;
      int so, sl, no, nl;
      if (i > 0) ss >> comma; // consume ", "
      ss >> paren >> so >> comma >> sl >> comma
         >> no >> comma >> nl >> paren;
      g->nodes[i].seq_off = (int16_t)so;
      g->nodes[i].seq_len = (int16_t)sl;
      g->nodes[i].next_off = (int16_t)no;
      g->nodes[i].next_len = (int16_t)nl;
    }
  }

  // Parse nexts
  g->nexts = (int16_t*)malloc(
      g->total_nexts * sizeof(int16_t));
  std::getline(f, line);
  {
    std::istringstream ss(line);
    for (uint32_t i = 0; i < g->total_nexts; i++){
      int v; ss >> v;
      g->nexts[i] = (int16_t)v;
    }
  }

  // Parse sequences
  g->seqs = (int8_t*)malloc(g->total_seq * sizeof(int8_t));
  std::getline(f, line);
  {
    std::istringstream ss(line);
    for (uint32_t i = 0; i < g->total_seq; i++){
      int v; ss >> v;
      g->seqs[i] = (int8_t)v;
    }
  }

  return g;
}

// Load first num_reads reads from reads.txt in a single pass
static std::vector<std::string> ld_seqs(
    const std::string& input_dir, size_t num_reads){
  std::vector<std::string> seqs;
  seqs.reserve(num_reads);
  std::ifstream f(input_dir + "/Inputs/reads.txt");
  std::string line;
  for (size_t i = 0; i < num_reads && std::getline(f, line); i++){
    // Strip the line number prefix ("123: ")
    size_t space_pos = line.find(' ');
    if (space_pos != std::string::npos)
      seqs.push_back(line.substr(space_pos + 1));
    else
      seqs.push_back(line);
  }
  return seqs;
}

std::vector<ReadAlignmentParams>* load_read_alignment_params(
    size_t num_inputs, std::string input_dir){

  if (!text_data_exists(input_dir)){
    fprintf(stderr,
        "error: text data (graph.soa, matchProfiles.txt) "
        "not found in %s/Inputs/\n"
        "Run a prior branch with DUMP_GRAPH to generate.\n",
        input_dir.c_str());
    exit(1);
  }

  int skipped = 0;

  // Load graphs + profiles from text
  std::cout << "Loading from text files..." << std::endl;
  std::ifstream gf(input_dir + "/Inputs/graph.soa");
  std::ifstream pf(input_dir + "/Inputs/matchProfiles.txt");
  uint32_t ng, np;
  gf >> ng; gf.ignore();
  pf >> np; pf.ignore();
  size_t limit = std::min(num_inputs,
                          (size_t)std::min(ng, np));

  // Still need reads for N-filtering
  std::vector<std::string> seqs = ld_seqs(input_dir, limit);
  limit = std::min(limit, seqs.size());

  auto* params = new std::vector<ReadAlignmentParams>(limit);
  size_t out_idx = 0;
  for (size_t i = 0; i < limit; i++){
    gssw_soa_graph* g = load_graph_text(gf);
    gssw_profile* p = load_profile_text(pf);

    // N-filter
    if (read_has_n(seqs[i]) || soa_graph_has_n(g)){
      gssw_soa_graph_destroy(g);
      gssw_init_destroy(p);
      skipped++;
      continue;
    }

    (*params)[out_idx].graph = g;
    (*params)[out_idx].prof = p;
    out_idx++;
  }
  params->resize(out_idx);

  if (skipped > 0){
    std::cout << "Filtered " << skipped
              << " queries containing N" << std::endl;
  }
  return params;
}

int ld_num_inputs(std::string in_dir){
  std::string data("");
  std::string lineNum("");
  std::ifstream f(in_dir+"/Inputs/reads.txt");

  //scan up until the appropriate line
  std::string line("");
  int i = 0;
  while (std::getline(f,line)){i++;}
  return i;
}
