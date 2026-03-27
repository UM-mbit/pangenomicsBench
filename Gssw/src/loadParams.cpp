#include "loadParams.h"
#include <cassert>
#include <stdint.h>
#include <cstdlib>
#include <cstdio>
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
  delete[] nt_table;
  delete[] score_matrix;
}

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

// ---- Binary dump/load helpers ----

#ifdef DUMP_GRAPH
// Write all graphs and reads to compact binary files
static void dump_binary(const std::string& input_dir,
                        const std::vector<gssw_soa_graph*>& graphs,
                        const std::vector<std::string>& reads){
  // Write graph.bin
  std::string gpath = input_dir + "/Inputs/graph.bin";
  FILE* gf = fopen(gpath.c_str(), "wb");
  if (!gf){
    std::cerr << "Failed to open " << gpath << std::endl;
    return;
  }
  uint32_t n = graphs.size();
  fwrite(&n, sizeof(uint32_t), 1, gf);
  for (uint32_t i = 0; i < n; i++){
    gssw_soa_graph* g = graphs[i];
    fwrite(&g->num_nodes, sizeof(uint32_t), 1, gf);
    fwrite(&g->total_nexts, sizeof(uint32_t), 1, gf);
    fwrite(&g->total_seq, sizeof(uint32_t), 1, gf);
    fwrite(g->nodes, sizeof(gssw_node_desc), g->num_nodes, gf);
    fwrite(g->nexts, sizeof(int16_t), g->total_nexts, gf);
    fwrite(g->seqs, sizeof(int8_t), g->total_seq, gf);
  }
  fclose(gf);

  // Write reads.bin
  std::string rpath = input_dir + "/Inputs/reads.bin";
  FILE* rf = fopen(rpath.c_str(), "wb");
  if (!rf){
    std::cerr << "Failed to open " << rpath << std::endl;
    return;
  }
  uint32_t nr = reads.size();
  fwrite(&nr, sizeof(uint32_t), 1, rf);
  for (uint32_t i = 0; i < nr; i++){
    uint32_t len = reads[i].size();
    fwrite(&len, sizeof(uint32_t), 1, rf);
    fwrite(reads[i].c_str(), 1, len, rf);
  }
  fclose(rf);
}
#endif // DUMP_GRAPH

// Check if binary graph file exists
static bool binary_exists(const std::string& input_dir){
  std::string gpath = input_dir + "/Inputs/graph.bin";
  std::string rpath = input_dir + "/Inputs/reads.bin";
  std::ifstream gf(gpath);
  std::ifstream rf(rpath);
  return gf.good() && rf.good();
}

// Load a single SoA graph from an open binary file
static gssw_soa_graph* load_graph_binary(FILE* f){
  gssw_soa_graph* g =
      (gssw_soa_graph*)calloc(1, sizeof(gssw_soa_graph));
  fread(&g->num_nodes, sizeof(uint32_t), 1, f);
  fread(&g->total_nexts, sizeof(uint32_t), 1, f);
  fread(&g->total_seq, sizeof(uint32_t), 1, f);
  g->nodes = (gssw_node_desc*)malloc(
      g->num_nodes * sizeof(gssw_node_desc));
  fread(g->nodes, sizeof(gssw_node_desc), g->num_nodes, f);
  g->nexts = (int16_t*)malloc(
      g->total_nexts * sizeof(int16_t));
  fread(g->nexts, sizeof(int16_t), g->total_nexts, f);
  g->seqs = (int8_t*)malloc(g->total_seq * sizeof(int8_t));
  fread(g->seqs, sizeof(int8_t), g->total_seq, f);
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
  // ---- Dump mode: load ALL from JSON, convert, write binary ----
  {
    std::cout << "DUMP_GRAPH: loading all JSON..." << std::endl;
    nlohmann::json* graphs = ld_gssw_graph(input_dir);
    size_t json_size = graphs->size();
    std::vector<std::string> all_seqs =
        ld_seqs(input_dir, json_size);
    std::vector<gssw_soa_graph*> all_graphs;
    all_graphs.reserve(json_size);
    for (size_t i = 0; i < json_size; i++){
      gssw_graph* old_g = ld_graph((*graphs)[i]);
      all_graphs.push_back(convert_to_soa(old_g));
      gssw_graph_destroy(old_g);
    }
    delete graphs;
    dump_binary(input_dir, all_graphs, all_seqs);
    std::cout << "Dumped " << json_size
              << " graphs to binary" << std::endl;
    for (auto g : all_graphs) gssw_soa_graph_destroy(g);
  }
  // Fall through to binary loading below
#endif

  int skipped = 0;
  std::vector<ReadAlignmentParams>* params;

  if (binary_exists(input_dir)){
    // ---- Fast path: load from binary ----
    std::cout << "Loading from binary..." << std::endl;
    std::string gpath = input_dir + "/Inputs/graph.bin";
    std::string rpath = input_dir + "/Inputs/reads.bin";
    FILE* gf = fopen(gpath.c_str(), "rb");
    FILE* rf = fopen(rpath.c_str(), "rb");
    uint32_t ng, nr;
    fread(&ng, sizeof(uint32_t), 1, gf);
    fread(&nr, sizeof(uint32_t), 1, rf);
    size_t limit = std::min(num_inputs,
                            (size_t)std::min(ng, nr));
    params = new std::vector<ReadAlignmentParams>(limit);
    size_t out_idx = 0;
    for (size_t i = 0; i < limit; i++){
      // Read the read
      uint32_t rlen;
      fread(&rlen, sizeof(uint32_t), 1, rf);
      std::string seq(rlen, '\0');
      fread(&seq[0], 1, rlen, rf);

      // Read the graph
      gssw_soa_graph* g = load_graph_binary(gf);

      // N-filter
      if (read_has_n(seq) || soa_graph_has_n(g)){
        gssw_soa_graph_destroy(g);
        skipped++;
        continue;
      }

      (*params)[out_idx].graph = g;
      (*params)[out_idx].seq = seq;
      (*params)[out_idx].nt_table = get_nt_table(seq.size());
      (*params)[out_idx].score_matrix = get_score_matrix();
      out_idx++;
    }
    params->resize(out_idx);
    fclose(gf);
    fclose(rf);
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
      (*params)[out_idx].nt_table = get_nt_table(seqs[i].size());
      (*params)[out_idx].score_matrix = get_score_matrix();
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

int8_t* get_nt_table(size_t seqLen){
//This one is a little funky. In vg it is initialized according to a constant
//pattern that uses some for loops. Here we've just taken the first 255 elements
//and will use substrings to generate however many are needed
  static int8_t full_nt_table[255]{4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 4, 1, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 0, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 71, 67, 84, 67, 0, -15, 32, -96, -94, 127, 0, 0, -128, -53, 32, -96, -94, 127, 0, 0, -128, -105, 33, -96, -94, 127, 0, 0, 48, -1, -122, -96, -94, 127, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 83, 50, 50, 53, 95, 52, 56, 50, 49, 52, 57, 0, -2, 127, 0, 0, 80, -1, -122, -96, -94, 127, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 83, 50, 50, 53, 95, 52, 56, 50, 49, 52, 57, 0, 71, 84, 67, 71, -92, 0, 0, 0, 65, 71, 65, 67, 13, 0, 0, 0, 0, 0, 0, 0, -92, 0, 0, 0, -91, 0, 0, 0, 0, 4, 4, 4, 4, 4};
  int8_t* nt_table = new int8_t[255];
  for (int i = 0; i < seqLen; i++){
    nt_table[i] = full_nt_table[i];
  }
  return nt_table;
}

int8_t* get_score_matrix(){
  int8_t* score_matrix = new int8_t[25]{1,-4,-4,-4, 0,
                                       -4, 1,-4,-4, 0,
                                       -4,-4, 1,-4, 0,
                                       -4,-4,-4, 1, 0,
                                        0, 0, 0, 0, 0};
  return score_matrix;
}

nlohmann::json* ld_gssw_graph(std::string in_dir){
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


std::string ld_seq(std::string in_dir, int ind){
  std::string data("");
  std::string lineNum("");
  std::ifstream f(in_dir+"/Inputs/reads.txt");

  //scan up until the appropriate line
  std::string line("");
  std::getline(f,line);
  int i = 0;
  while (i < ind) { std::getline(f,line); i++;}
  std::istringstream lineStream(line);
  
  //strip the number from line
  std::getline(lineStream, lineNum, ' ');
  //get the remainder (the data)
  std::getline(lineStream, data);

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
