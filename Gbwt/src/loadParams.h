#ifndef __loadParams_H__
#define __loadParams_H__

#include <vector>
#include <string>

#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"

/*
 * returns the first argment on the command line. This should be the directory
 * of the input files
 * @param int argc as passed to main
 * @param char* argv as passed to main
 * @return string the 1st command line arg, should be input dir
 */
std::string parseArgs(int argc, char* argv[]);

/*
 * loads in queries from a file
 * @param string inDir, the root directory of the Inputs
 * @return vector<vector<int>> each vector<int> is a list of node ids (a query)
 */
std::vector<std::vector<int>>* loadQueries(std::string inputDir, int numInputs);

/*
 * loads in the gbwt from a file
 * @param string inDir, the root directory of the Inputs
 * @return GBWT object initialized from the GBWT in the Inputs
 */
gbwt::GBWT* ldGbwt(std::string inputDir);

/*
 * Counts the number of reads to process
 * @param string inDir, the root directory of the Inputs
 * @return return the number of reads we're running
 */
int ldNumInputs(std::string in_dir);


#endif // __loadParams_H__
