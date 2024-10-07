#include <iostream>
#include <vector>

#include "paf.hpp"


using namespace std;

const string PAF_PATH = "/data2/jnms/chr20/chr20.paf";

int main(void) {
    std::cout << "hello, world!" << std::endl;

    std::vector<std::pair<std::string, uint64_t>> pafs_and_min_lengths;
    pafs_and_min_lengths = seqwish::parse_paf_spec(PAF_PATH);


    for (auto &e : pafs_and_min_lengths) {
        std::cout << e.first << " " << e.second << std::endl;
    }


    /*
    size_t graph_length = compute_transitive_closures(seqidx, aln_iitree, seq_v_file, node_iitree, path_iitree,
                                                      args::get(repeat_max),
                                                      args::get(min_repeat_dist),
                                                      transclose_batch ? (uint64_t)seqwish::handy_parameter(args::get(transclose_batch), 1000000) : 1000000,
                                                      args::get(show_progress),
                                                      num_threads,
                                                      start_time);
    */
    return 0;
}
