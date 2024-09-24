#ifndef __loadParams_H__
#define __loadParams_H__

#include <stdint.h>
#include <cstdlib>
#include <string>
#include <vector>

#include "GraphAlignerCommon.h"
#include "GraphAlignerWrapper.h"
#include "GraphAlignerBitvectorBanded.h"

using Common = GraphAlignerCommon<size_t, int64_t, uint64_t>;
using Params = typename Common::Params;
using SerializableParams = typename Common::SerializableParams;
using ReusableState = typename Common::AlignerGraphsizedState; 
using OnewayTrace = typename Common::OnewayTrace;
using BitvectorAligner=GraphAlignerBitvectorBanded<size_t, int64_t, uint64_t>;

const int WORD_SIZE = 64;
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
 * Loads in the params object. Because of peculiarities of the constructors, the
 * graph is "owned" by SerializableParams. Params. only holds a reference. The
 * class we want is Params, but we need to keep SerializableParams around so we
 * don't destroy the graph. Hence the pair return type.
 * @params std::string inputDir the path to Input folder (not including input)
 * @returns Params* and SerializableParams* SerializableParams has the data.
 *   Params is a copy that references some parts of SerializableParams
 */
std::pair<Params*, SerializableParams*> loadParams(std::string inputDir);

/*
 * Load a vector of sequences from the read file
 * @param string inputDir the path to the Input folder
 * @return vector<string>* the reads
 */
std::vector<std::string>* loadSequences(std::string inputDir);

/*
 * Reverse the strings in a vector load a vector of sequences from the read file
 * @param string the original sequences
 * @return vector<string>* reversed strings
 */
std::vector<std::string>* getRevSequence(std::vector<std::string>* seqs);

/*
 * Loads seed hits (clusters) from json
 * @param string inputDir the path to Input folder
 * @return a vector of clusters
 */
std::vector<std::vector<ProcessedSeedHit>>* loadSeedHits(
    std::string inputDir, size_t numInputs);


/*
 * Calculates the number of slices (parameter to the kernel). This correpsonds
 * to the number of strips we divide the DP matrix into
 * @param std::string seq
 * @return size_t the number of slices
 */
size_t calcNumSlices(std::string& seq);


/*
 * Initializes a reusable state from params
 * @param Params (we use graph and bandwidth from here)
 * @returns ReusableState used in the kernel
 */
ReusableState* getReusableState(Common::Params* params);

/*
 * Get max scores for slices
 * @param std::string the input directory
 * @param the number of inputs
 * @returns vector<vector<int64_t> vector of max scores
 */
std::vector<std::vector<int64_t>>* getSliceMaxScores(
    std::string inputDir, size_t numInputs);

/*
 * Counts the number of reads to process
 * @param string inDir, the root directory of the Inputs
 * @return return the number of reads we're running
 */
int ld_num_inputs(std::string in_dir);


#endif // __loadParams_H__
