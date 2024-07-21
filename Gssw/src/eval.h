#ifndef __EVAL_H__
#define __EVAL_H__
#include <string>
#include "gssw.h"

void write_ggm(std::string out_dir, int ind, gssw_graph_mapping* ggm);

void init_output_dir(std::string output_dir);

#endif //__EVAL_H__
