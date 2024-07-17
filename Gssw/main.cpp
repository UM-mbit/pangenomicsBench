#include <iostream>
#include <cstring>

#include "deps/Gssw/src/gssw.h"
#include "loadParams.h"
#include "eval.h"

#define INPUT_DIR "/data2/kaplannp/Genomics/Datasets/Kernels/Gssw"
#define OUT_DIR "Out"

int main(){

  int num_inputs = ld_num_inputs(INPUT_DIR);
  std::ofstream out_file;
  //For loop over reads.
  for (int i=0; i < num_inputs; i++){
    //load inputs
    gssw_graph* graph = ld_graph(INPUT_DIR, i);
    char* seq = ld_seq(INPUT_DIR, i);
    const int8_t* nt_table = get_nt_table(std::strlen(seq));

    //run the kernel
    gssw_graph_fill_pinned( graph,
                            seq,
                            nt_table,
                            constant::score_matrix
                            constant::weight_gapO, 
                            constant::weight_gapE, 
                            constant::start_full_length_bonus,
                            constant::end_full_length_bonus,
                            constant::maskLen,
                            constant::score_size,
                            constant::save_matrixes );

    //generate outputs
    gssw_graph_mapping* ggm = gssw_graph_trace_back( 
                                   graph,
                                   seq,
                                   strlen(seq),
                                   nt_table,
                                   constant::score_matrix,
                                   constant::weight_gapO, 
                                   constant::weight_gapE, 
                                   constant::start_full_length_bonus,
                                   constant::start_full_length_bonus);
    //write outputs
    write_ggm(out_file, ggm);

    //cleanup
    gssw_graph_destroy(graph);
    gssw_graph_mapping_destroy(ggm);
    delete seq;
    delete nt_table;
  }
}

