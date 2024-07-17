#ifndef __loadParams_H__
#define __loadParams_H__

namespace constants{
  const int8_t* score_matrix = new int8_t[4]{1, -4, -4, -4};
  const uint8_t weight_gapO = 6;
  const uint8_t weight_gapE = 1;
  const int8_t start_full_length_bonus = 5;
  const int8_t end_full_length_bonus = 5;
  const int32_t maskLen = 15;
  const int8_t score_size = 2;
  bool save_matrixes = 1;
}

const int8_t* get_nt_table(size_t seqLen);

/*
 * Load the gssw graph for this iteration
 * @param string inDir, the root directory of the Inputs
 * @param int ind, the index of this iteration
 * @retun gssw_graph*, ptr the loaded graph
 */
gssw_graph* ld_graph(std::string in_dir, int ind);

/*
 * Load the sequence for this iteration
 * @param string inDir, the root directory of the Inputs
 * @param int ind, the index of this iteration
 * @return char*, sequence as a cstr
 */
char* ld_seq(std::string in_dir, int ind);

/*
 * Counts the number of reads to process
 * @param string inDir, the root directory of the Inputs
 * @return return the number of reads we're running
 */
int ld_num_inputs(std::string in_dir);


#endif // __loadParams_H__
