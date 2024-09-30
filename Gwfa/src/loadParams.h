#ifndef LOAD_PARAMS_H
#define LOAD_PARAMS_H
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include "gfa.h"
#include "gfa-priv.h"
#include "kalloc.h"

//In the kernel this is passed as a parameter, but here we use the default
const int32_t GDP_MAX_ED = 10000;

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
 * returns the first argment on the command line. This should be the directory
 * of the input files
 * @param int argc as passed to main
 * @param char* argv as passed to main
 * @return string the 1st command line arg, should be input dir
 */
std::string parseArgs(int argc, char* argv[]);

/*
 * Generic function for loading inputs if the format of the file is one input
 * per line of a simple type (e.g. uint32_t or int32_t.)
 */
template<typename T>
std::vector<T>* loadScalarInput(std::string filepath){
  std::vector<T>* inputs = new std::vector<T>();
  std::string line("");
  std::ifstream f(filepath);
  while (std::getline(f, line)){
    inputs->push_back(static_cast<T>(std::stoll(line)));
  }
  return inputs;
}

/*
 * The previous function doesn't quite generalize for strings, so we build
 * essentially a copy here that will do strings
 */
std::vector<std::string>* loadStrInput(std::string filepath);

/*
 * initializes es from the graph
 */
const gfa_edseq_t* loadEs(gfa_t* g);

/*
 * initialize a bunch of kalloc memory managers. I don't mean to be opaque, but
 * I have little inclination as to what these km objects actually do.
 * @param size the number of km objects to initialize
 */
std::vector<void*>* initKms(uint32_t size);

/*
 * Z is some kind of void* struct which includes a lot of useful inputs. It's
 * not clear why it is called z. I follow the conventions from minigraph
 */
std::vector<void*>* constructZs( std::vector<void*>* kmBuff,
                                 gfa_edopt_t* optBuff,
                                 gfa_t* graph,
                                 gfa_edseq_t* es,
                                 std::vector<uint32_t>* queryGapLenBuff,
                                 std::vector<std::string>* queryGapBuff,
                                 std::vector<uint32_t>* v0Buff,
                                 std::vector<int32_t>* end0Buff);

/*
 * Initializes options for the kerenl with the same values that are used in the
 * minigraph call
 */
gfa_edopt_t* initOpt();

#endif//  LOAD_PARAMS_H
