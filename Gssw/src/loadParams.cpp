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

#include "gssw_to_json.hpp"

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
}

// Forward declarations for internal functions
static nlohmann::json* ld_gssw_graph(std::string in_dir);

// Check if a read sequence contains N or n
static bool read_has_n(const std::string& seq){
  for (char c : seq){
    if (c == 'N' || c == 'n') return true;
  }
  return false;
}

// Check if any node in the graph has N (num value 4) in its sequence
static bool graph_has_n(gssw_graph* g){
  for (uint32_t i = 0; i < g->size; i++){
    gssw_node* n = g->nodes[i];
    for (int32_t j = 0; j < n->len; j++){
      if (n->num[j] == 4) return true;
    }
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

// ---- Text-based SoA graph dump/load ----

#ifdef DUMP_GRAPH
// Write all SoA graphs to a plain text file (graph.soa).
//
// File format:
//   Line 1: <num_graphs>            (total number of graphs)
//   Then for each graph:
//     Line 1: <num_nodes> <total_nexts> <total_seq>
//     Line 2: (seq_off,seq_len,next_off,next_len), ...
//             One tuple per node, comma-separated.
//             seq_off/seq_len index into the sequence line.
//             next_off/next_len index into the nexts line.
//     Line 3: <child_id> <child_id> ...
//             Space-separated child node indices (topo order).
//             Length = total_nexts.
//     Line 4: <base> <base> ...
//             Space-separated numeric bases (A=0,C=1,G=2,T=3).
//             Length = total_seq.
//
// Reads are stored separately in reads.txt (one per line,
// prefixed with "<index>: ").
static void dump_text(const std::string& input_dir,
                      const std::vector<gssw_soa_graph*>& graphs){
  std::string path = input_dir + "/Inputs/graph.soa";
  std::ofstream f(path);
  if (!f.is_open()){
    std::cerr << "Failed to open " << path << std::endl;
    return;
  }
  f << graphs.size() << "\n";
  for (size_t gi = 0; gi < graphs.size(); gi++){
    gssw_soa_graph* g = graphs[gi];
    f << g->num_nodes << " "
      << g->total_nexts << " "
      << g->total_seq << "\n";
    // Node descriptors
    for (uint32_t i = 0; i < g->num_nodes; i++){
      if (i > 0) f << ", ";
      f << "(" << g->nodes[i].seq_off << ","
        << g->nodes[i].seq_len << ","
        << g->nodes[i].next_off << ","
        << g->nodes[i].next_len << ")";
    }
    f << "\n";
    // Nexts
    for (uint32_t i = 0; i < g->total_nexts; i++){
      if (i > 0) f << " ";
      f << g->nexts[i];
    }
    f << "\n";
    // Sequences (numeric values)
    for (uint32_t i = 0; i < g->total_seq; i++){
      if (i > 0) f << " ";
      f << (int)g->seqs[i];
    }
    f << "\n";
  }
}
#endif // DUMP_GRAPH

// Check if text graph file exists
static bool text_graph_exists(const std::string& input_dir){
  std::string path = input_dir + "/Inputs/graph.soa";
  std::ifstream f(path);
  return f.good();
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

#ifdef DUMP_GRAPH
  // ---- Dump mode: load ALL from JSON, convert, write text ----
  {
    std::cout << "DUMP_GRAPH: loading all JSON..." << std::endl;
    nlohmann::json* graphs = ld_gssw_graph(input_dir);
    size_t json_size = graphs->size();
    std::vector<gssw_soa_graph*> all_graphs;
    all_graphs.reserve(json_size);
    for (size_t i = 0; i < json_size; i++){
      gssw_graph* old_g = ld_graph((*graphs)[i]);
      all_graphs.push_back(convert_to_soa(old_g));
      gssw_graph_destroy(old_g);
    }
    delete graphs;
    dump_text(input_dir, all_graphs);
    std::cout << "Dumped " << json_size
              << " graphs to graph.soa" << std::endl;
    for (auto g : all_graphs) gssw_soa_graph_destroy(g);
  }
  // Fall through to text loading below
#endif

  int skipped = 0;
  std::vector<ReadAlignmentParams>* params;

  if (text_graph_exists(input_dir)){
    // ---- Fast path: load from text SoA + reads.txt ----
    std::cout << "Loading from graph.soa..." << std::endl;
    std::string gpath = input_dir + "/Inputs/graph.soa";
    std::ifstream gf(gpath);
    uint32_t ng;
    gf >> ng;
    gf.ignore();

    std::vector<std::string> seqs = ld_seqs(input_dir, ng);
    size_t limit = std::min(num_inputs,
                            (size_t)std::min(ng,
                              (uint32_t)seqs.size()));

    params = new std::vector<ReadAlignmentParams>(limit);
    size_t out_idx = 0;
    for (size_t i = 0; i < limit; i++){
      gssw_soa_graph* g = load_graph_text(gf);

      // N-filter
      if (read_has_n(seqs[i]) || soa_graph_has_n(g)){
        gssw_soa_graph_destroy(g);
        skipped++;
        continue;
      }

      (*params)[out_idx].graph = g;
      (*params)[out_idx].seq = seqs[i];
      out_idx++;
    }
    params->resize(out_idx);
  } else {
    // ---- Slow path: load from JSON ----
    std::cout << "Loading from JSON..." << std::endl;
    nlohmann::json* graphs = ld_gssw_graph(input_dir);
    size_t json_size = graphs->size();
    size_t limit = std::min(num_inputs, json_size);

    std::vector<std::string> seqs = ld_seqs(input_dir, limit);
    limit = std::min(limit, seqs.size());

    params = new std::vector<ReadAlignmentParams>(limit);
    size_t out_idx = 0;
    for (size_t i = 0; i < limit; i++){
      gssw_graph* old_g = ld_graph((*graphs)[i]);

      if (read_has_n(seqs[i]) || graph_has_n(old_g)){
        gssw_graph_destroy(old_g);
        skipped++;
        continue;
      }

      gssw_soa_graph* g = convert_to_soa(old_g);
      gssw_graph_destroy(old_g);

      (*params)[out_idx].graph = g;
      (*params)[out_idx].seq = seqs[i];
      out_idx++;
    }
    params->resize(out_idx);
    delete graphs;
  }

  if (skipped > 0){
    std::cout << "Filtered " << skipped
              << " queries containing N" << std::endl;
  }
  return params;
}

// Encode ASCII read to numeric (A=0, C=1, G=2, T=3)
std::vector<int8_t> encode_read(const std::string& seq) {
  std::vector<int8_t> num(seq.size());
  for (size_t i = 0; i < seq.size(); i++) {
    switch (seq[i]) {
      case 'A': case 'a': num[i] = 0; break;
      case 'C': case 'c': num[i] = 1; break;
      case 'G': case 'g': num[i] = 2; break;
      case 'T': case 't': num[i] = 3; break;
      default: num[i] = 0; break; // N filtered out already
    }
  }
  return num;
}

static nlohmann::json* ld_gssw_graph(std::string in_dir){
  //std::cerr << "about to open the file" << std::endl;
  std::ifstream f(in_dir+"/Inputs/graph.json");
  //std::cerr << in_dir+"/Inputs/Graphs/g"+std::to_string(ind)+".json" << std::endl;
  if (!f.is_open()) {
              throw std::runtime_error("Could not open file");
  }
  nlohmann::json* data = new nlohmann::json();
  *data = nlohmann::json::parse(f);
  return data;
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
