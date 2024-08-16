#ifndef __loadParams_H__
#define __loadParams_H__

#include <stdint.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "gssw.h"
 
namespace constants{
  const uint8_t weight_gapO = 6;
  const uint8_t weight_gapE = 1;
  const int8_t start_full_length_bonus = 5;
  const int8_t end_full_length_bonus = 5;
  const int32_t maskLen = 15;
  const int8_t score_size = 2;
  const bool save_matrixes = 1;
}

/*
 * Convenient structure for holding the parameters needed for a single iteration
 * (read) of gssw.
 */
typedef struct ReadAlignmentParams {
  gssw_graph* graph;
  std::string seq;
  int8_t* nt_table;
  int8_t* score_matrix;
  ~ReadAlignmentParams();
} ReadAlignmentParams;

/*
 * returns the first argment on the command line. This should be the directory
 * of the input files
 * @param int argc as passed to main
 * @param char* argv as passed to main
 * @return string the 1st command line arg, should be input dir
 */
std::string parseArgs(int argc, char* argv[]);

/*
 */
std::vector<ReadAlignmentParams>* load_read_alignment_params(size_t num_inputs,
                                                        std::string input_dir);


/*
 * At a glance:
 * This is a semi constant input to kernel, the size varies with the sequence
 * length though
 * In more depth
 * In vg this is generated according to a constant formula/patttern using some
 * for loops, I just keep a hardcoded list and get substrings of it.
 * @param size_t seqLen the length of the sequence
 * @returns int_8* nt_table array input for gssw
 */
int8_t* get_nt_table(size_t seqLen);

/*
 * This is constant, but gssw wants it as a non const object, though I don't
 * believe it changes it. To be safe this copies the object so there is no
 * chance of modifying it
 * @returns score matrix {1,-4,-4,-4}
 */
int8_t* get_score_matrix();

/*
 * Load the gssw graph for this iteration
 * @param string inDir, the root directory of the Inputs
 * @param int ind, the index of this iteration
 * @retun gssw_graph*, ptr the loaded graph
 */
gssw_graph* ld_gssw_graph(std::string in_dir, int ind);

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
