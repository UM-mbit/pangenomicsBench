#ifndef __EVAL_H__
#define __EVAL_H__
#include <string>
#include <fstream>
#include <vector>
#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"

void init_output_dir(std::string output_dir);

/*
 * convert gbwt::SearchState to a string
 * @param gbwt::SearchState the output of a prefix query
 * @returns string representation of the query. "node: (startRange,endRange)"
 */
std::string searchStateToStr(gbwt::SearchState state);

/*
 * writes the query results to a file 
 * @param outDir the ouput directory
 * @param vector<gbwt::searchState> the outputs of the queries
 */
void dumpStatesToFile(std::string outDir, 
      std::vector<gbwt::SearchState> queryOuts);

#endif //__EVAL_H__
