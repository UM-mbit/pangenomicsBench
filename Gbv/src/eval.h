#ifndef __EVAL_H__
#define __EVAL_H__
#include <string>
#include <fstream>
#include <vector>

#include "loadParams.h"

void init_output_dir(std::string output_dir);

void writeTraces(std::vector<std::vector<OnewayTrace>>& traces,
                 std::ostream& file);

#endif //__EVAL_H__
