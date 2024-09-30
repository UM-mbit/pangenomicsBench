#ifndef __loadParams_H__
#define __loadParams_H__

#include <vector>
#include <string>

#include "gbwt/gbwt.h"
#include "gbwt/algorithms.h"


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
 * loads in queries from a file
 * @param string inDir, the root directory of the Inputs
 * @return vector<vector<int>> each vector<int> is a list of node ids (a query)
 */
std::vector<std::vector<gbwt::node_type>>* loadQueries(std::string inputDir, int numInputs);

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
