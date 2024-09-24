#ifndef EVAL_H
#define EVAL_H
#include <vector>
#include <string>
#include "gfa-priv.h"
/*
 * This function attempts to mimic the serialization script within the
 * minigraphDumper repo. That script was not written to be reused, it was
 * written in C. This is a c++ version
 * @param results: a vector of gfa_edrst_t* that will be written to output file
 * @param output_file: string, the file to write the results to
 */
void writeResults(std::vector<gfa_edrst_t>* results, std::string output_file);

/*
 * Initializes the ouput directory
 * @param output_dir: string, the directory to create
 */
void init_output_dir(std::string output_dir);
#endif // EVAL_H
