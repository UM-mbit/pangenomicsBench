#ifndef __loadParams_H__
#define __loadParams_H__

#include <vector>
#include <string>
/*
 * loads in queries from a file
 * @param string inDir, the root directory of the Inputs
 * @return vector<vector<int>> each vector<int> is a list of node ids (a query)
 */
std::vector<std::vector<int>>* loadQueries(std::string inputDir, int numInputs);

/*
 * Counts the number of reads to process
 * @param string inDir, the root directory of the Inputs
 * @return return the number of reads we're running
 */
int ld_num_inputs(std::string in_dir);


#endif // __loadParams_H__
