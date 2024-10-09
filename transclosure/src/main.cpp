#include <iostream>
#include <vector>
#include <chrono>

//#include "gzstream.h"
//#include "paf.hpp"
#include "seqindex.hpp"
#include "tempfile.hpp"
#include "mmmultimap.hpp"

#include "alignments.hpp"
#include "transclosure.hpp"


using namespace std;

const string PAN_FASTA_PATH = "/data2/jnms/chr20/chr20.pan.fasta";
const string PAF_PATH = "/data2/jnms/chr20/chr20.paf";

int main(void) {
    std::cout << "Transclosure benchmark:" << std::endl;

    const int NTHREADS = 8;

    /*
    std::vector<std::pair<std::string, uint64_t>> pafs_and_min_lengths;
    pafs_and_min_lengths = seqwish::parse_paf_spec(PAF_PATH);


    for (auto &e : pafs_and_min_lengths) {
        std::cout << e.first << " " << e.second << std::endl;
    }
    */

    /*
    igzstream paf_in(PAF_PATH.c_str());
    while (!paf_in.eof()) {
        std::string line;
        std::getline(paf_in, line);

        if (!line.empty()) {
            seqwish::paf_row_t paf(line);

            if (paf.cigar.empty()){
                std::cerr << "[TRANSCLOSURE] WARNING: input alignment file " << PAF_PATH<< " does not have CIGAR strings. " << std::endl;
            }
            break;
        }
    }
    */

    char* cwd = get_current_dir_name();
    temp_file::set_dir(std::string(cwd));
    free(cwd);
    temp_file::set_keep_temp(true);

    // build seqidx
    auto seqidx_ptr = std::make_unique<seqwish::seqindex_t>();
    auto& seqidx = *seqidx_ptr;
    seqidx.build_index(PAN_FASTA_PATH);
    seqidx.save();

    // parse alignments
    const std::string aln_idx = temp_file::create("seqwish-", ".sqa");
    auto aln_iitree_ptr = std::make_unique<mmmulti::iitree<uint64_t, seqwish::pos_t>>(aln_idx);
    auto& aln_iitree = *aln_iitree_ptr;
    aln_iitree.open_writer();
    float sparse_match = 0;
    auto& file = PAF_PATH;
    uint64_t min_length = 0;
    seqwish::unpack_paf_alignments(file, aln_iitree, seqidx, min_length, sparse_match, NTHREADS);

    aln_iitree.index(NTHREADS);

    // transclosure
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
    const std::string seq_v_file = temp_file::create("seqwish-", ".sqs");
    const std::string node_iitree_idx = temp_file::create("seqwish-", ".sqn");
    const std::string path_iitree_idx = temp_file::create("seqwish-", ".sqp");
    auto node_iitree_ptr = std::make_unique<mmmulti::iitree<uint64_t, seqwish::pos_t>>(node_iitree_idx); // maps graph seq to input seq
    auto& node_iitree = *node_iitree_ptr;
    auto path_iitree_ptr = std::make_unique<mmmulti::iitree<uint64_t, seqwish::pos_t>>(path_iitree_idx); // maps input seq to graph seq
    auto& path_iitree = *path_iitree_ptr;
    size_t graph_length = compute_transitive_closures(seqidx, aln_iitree, seq_v_file, node_iitree, path_iitree,
                                                      uint64_t{0},
                                                      uint64_t{0},
                                                      uint64_t{1000000},
                                                      false,
                                                      NTHREADS,
                                                      start_time);
    std::cout << graph_length << std::endl;

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
