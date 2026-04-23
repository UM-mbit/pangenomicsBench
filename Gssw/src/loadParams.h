#ifndef __loadParams_H__
#define __loadParams_H__

#include <stdint.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "gssw.h"

/*
 * Convenient structure for holding the parameters needed for a single iteration
 * (read) of gssw.
 */
typedef struct ReadAlignmentParams {
  gssw_soa_graph* graph;
  gssw_profile* prof;  // precomputed match profile
  uint16_t score;      // filled by kernel
  ReadAlignmentParams() : graph(nullptr), prof(nullptr), score(0) {}
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


// Counts the number of reads to process
int ld_num_inputs(std::string in_dir);


#endif // __loadParams_H__
