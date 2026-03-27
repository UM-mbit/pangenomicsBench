#ifndef __loadParams_H__
#define __loadParams_H__

#include <stdint.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "gssw.h"
#include "nlohmann/json.hpp"
 
/*
 * Convenient structure for holding the parameters needed for a single iteration
 * (read) of gssw.
 */
typedef struct ReadAlignmentParams {
  gssw_soa_graph* graph;
  std::string seq;
  uint16_t score;  // filled by kernel
  ReadAlignmentParams() : graph(nullptr), score(0) {}
  ~ReadAlignmentParams();
} ReadAlignmentParams;

/*
 * Get the number of iterations to run from the command line arguments.
 * Throws an error if the number of arguments is not 2 or 3.
 * Default if only 2 arguments (so n_iters not provided) is MAX_INT.
 * @param int argc, the number of arguments
 * @param char* argv[], the arguments
 * @return int, the number of iterations to run
 */
int getNumItersFromArgs(int argc, char* argv[]);

/*
 * Get the input directory from the command line arguments.
 * Throws an error if the number of arguments is not 2 or 3.
 * @param int argc, the number of arguments
 * @param char* argv[], the arguments
 * @return string, the input directory
 */
std::string getInputDirFromArgs(int argc, char* argv[]);

/*
 */
std::vector<ReadAlignmentParams>* load_read_alignment_params(size_t num_inputs,
                                                        std::string input_dir);


// Encode ASCII read to numeric (A=0, C=1, G=2, T=3)
std::vector<int8_t> encode_read(const std::string& seq);

/*
 * Load the gssw graph for this iteration
 * @param string inDir, the root directory of the Inputs
 * @retun gssw_graph*, ptr the loaded graph
 */
nlohmann::json* ld_gssw_graph(std::string in_dir);

/*
 * Load the sequence for this iteration
 * @param string inDir, the root directory of the Inputs
 * @param int ind, the index of this iteration
 * @return string, sequence 
 */
std::string ld_seq(std::string in_dir, int ind);

/*
 * Counts the number of reads to process
 * @param string inDir, the root directory of the Inputs
 * @return return the number of reads we're running
 */
int ld_num_inputs(std::string in_dir);


#endif // __loadParams_H__
