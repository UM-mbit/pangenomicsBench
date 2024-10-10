#include <iostream>
#include <vector>
#include <chrono>

#include "seqindex.hpp"
#include "tempfile.hpp"
#include "mmmultimap.hpp"
#include "mmmultiset.hpp"
#include "atomic_queue.h"
#include "ips4o.hpp"

#include "alignments.hpp"
#include "transclosure.hpp"
#include "compact.hpp"
#include "links.hpp"
#include "gfa.hpp"


using namespace std;
using namespace seqwish;

const string PAN_FASTA_PATH = "/data2/jnms/chr20/chr20.pan.fasta";
const string PAF_PATH = "/data2/jnms/chr20/chr20.paf";
const string GFA_PATH = "chr20.gfa";


size_t compute_transitive_closures_kernel(
        const seqindex_t& seqidx,
        mmmulti::iitree<uint64_t, pos_t>& aln_iitree, // input alignment matches between query seqs
        const std::string& seq_v_file,
        mmmulti::iitree<uint64_t, pos_t>& node_iitree, // maps graph seq ranges to input seq ranges
        mmmulti::iitree<uint64_t, pos_t>& path_iitree, // maps input seq ranges to graph seq ranges
        uint64_t num_threads) {

    uint64_t repeat_max = uint64_t{0};
    uint64_t min_repeat_dist = uint64_t{0};
    uint64_t transclose_batch_size = uint64_t{1000000};

    // open the writers in the iitrees
    node_iitree.open_writer();
    path_iitree.open_writer();
    // open seq_v_file
    std::ofstream seq_v_out(seq_v_file.c_str());
    // remember the elements of Q we've seen
    //std::cerr << "seq_size " << seqidx.seq_length() << std::endl;
    std::vector<bool> q_seen_bv(seqidx.seq_length());
    //atomicbitvector::atomic_bv_t q_seen_bv(seqidx.seq_length());
    uint64_t input_seq_length = seqidx.seq_length();
    // a buffer of ranges to write into our iitree, arranged by range ending position in Q
    // we flush those intervals that don't get extended into the next position in S
    // this maps from a position in Q (our input seqs concatenated, offset and orientation)
    // to a range (start and length) in S (our graph sequence vector)
    // we are mapping from the /last/ position in the matched range, not the first
    std::map<pos_t, range_t> range_buffer;
    uint64_t bases_seen = 0;
    std::thread* graph_writer = nullptr;
    //uint64_t last_seq_id = seqidx.seq_id_at(0);
    // collect based on a seed chunk of a given length
    for (uint64_t i = 0; i < input_seq_length; ) {
        // scan our q_seen_bv to find our next start
        //std::cerr << "closing\t" << i << std::endl;
        while (i < input_seq_length && q_seen_bv[i]) ++i;
        //std::cerr << "scanned_to\t" << i << std::endl;
        if (i >= input_seq_length) break; // we're done!

        // where our chunk begins
        uint64_t chunk_start = i;
        // extend until we've got chunk_size unseen bases (and where it ends (not past the end of the sequence))
        uint64_t bases_to_consider = 0;
        uint64_t chunk_end = chunk_start;
        while (bases_to_consider < transclose_batch_size && chunk_end < input_seq_length) {
            bases_to_consider += !q_seen_bv[chunk_end++];
        }

        // collect ranges overlapping, per thread to avoid contention
        // bits of sequence we've seen during this union-find chunk
        atomicbitvector::atomic_bv_t q_curr_bv(seqidx.seq_length());
        // shared work queues for our threads
        auto todo_in_ptr = new range_atomic_queue_t;
        auto& todo_in = *todo_in_ptr;
        auto todo_out_ptr = new range_atomic_queue_t;
        auto& todo_out = *todo_out_ptr;
        auto ovlp_q_ptr = new overlap_atomic_queue_t;
        auto& ovlp_q = *ovlp_q_ptr;
        std::deque<std::pair<pos_t, uint64_t>> todo; // intermediate buffer for master thread
        std::vector<std::pair<match_t, bool>> ovlp; // written into by the master thread

        // seed the initial ranges
        // the chunk range isn't an actual alignment, so we handle it differently
        for_each_fresh_range({chunk_start, chunk_end, 0}, q_seen_bv, [&](match_t b) {
                // the special case is handling ranges that have no matches
                // we need to close these even if they aren't matched to anything
                for (uint64_t j = b.start; j < b.end; ++j) {
                    assert(!q_seen_bv[j]);
                    q_curr_bv.set(j);
                }
                auto range = std::make_pair(make_pos_t(b.start, false), b.end - b.start);
                if (!todo_out.try_push(range)) {
                    todo.push_back(range);
                    //todo_seen.insert(range);
                }
            });
        std::atomic<bool> work_todo;
        std::vector<std::atomic<bool>> explorings(num_threads);
        work_todo.store(false);
        auto worker_lambda =
            [&](uint64_t tid) {
                //auto& ovlp = ovlps[tid];
                auto& exploring = explorings[tid];
                while (!work_todo.load()) {
                    std::this_thread::sleep_for(1ns);
                }
                exploring.store(true);
                std::pair<pos_t, uint64_t> item;
                while (work_todo.load()) {
                    if (todo_out.try_pop(item)) {
                        exploring.store(true);
                        auto& pos = item.first;
                        auto& match_len = item.second;
                        uint64_t n = !is_rev(pos) ? offset(pos) : offset(pos) - match_len + 1;
                        uint64_t range_start = n;
                        uint64_t range_end = n + match_len;
                        explore_overlaps({range_start, range_end, pos},
                                         q_seen_bv,
                                         q_curr_bv,
                                         aln_iitree,
                                         ovlp_q,
                                         todo_in);
                    } else {
                        exploring.store(false);
                        std::this_thread::sleep_for(0.00001ns);
                    }
                }
                exploring.store(false);
            };
        // launch our threads to expand the overlap set in parallel
        std::vector<std::thread> workers; workers.reserve(num_threads);
        for (uint64_t t = 0; t < num_threads; ++t) {
            workers.emplace_back(worker_lambda, t);
        }
        // manage the threads
        uint64_t empty_iter_count = 0;
        auto still_exploring = [&explorings]() {
            bool ongoing = false;
            for (auto& e : explorings) {
                ongoing = ongoing || e.load();
            }
            return ongoing;
        };
        work_todo.store(true);
        while (!todo_in.was_empty() || !todo.empty() || !todo_out.was_empty() || !ovlp_q.was_empty() || still_exploring() || ++empty_iter_count < 1000) {
            std::this_thread::sleep_for(0.00001ns);
            // read from todo_in, into todo
            std::pair<pos_t, uint64_t> item;
            while (todo_in.try_pop(item)) {
                todo.push_back(item);
            }
            // then transfer to todo_out, until it's full
            while (!todo.empty()) {
                empty_iter_count = 0;
                item = todo.front();
                if (todo_out.try_push(item)) {
                    todo.pop_front();
                } else {
                    break;
                }
            }
            // collect our overlaps
            std::pair<match_t, bool> o;
            while (ovlp_q.try_pop(o)) {
                ovlp.push_back(o);
            }
        }
        //std::cerr << "telling threads to stop" << std::endl;
        work_todo.store(false);
        assert(todo.empty() && todo_in.was_empty() && todo_out.was_empty() && ovlp_q.was_empty() && !still_exploring());
        //std::cerr << "gonna join" << std::endl;
        for (uint64_t t = 0; t < num_threads; ++t) {
            workers[t].join();
        }
        delete todo_in_ptr;
        delete todo_out_ptr;
        delete ovlp_q_ptr;

        // run the transclosure for this region using lock-free union find
        // convert the ranges into positions in the input sequence space
        // ... compute ranges
        std::vector<std::pair<uint64_t, uint64_t>> ranges;
        {
            uint64_t begin = 0;
            uint64_t end = q_curr_bv.size();
            uint64_t chunk_size = end/num_threads;

            std::pair<uint64_t, uint64_t> todo_range = std::make_pair(begin, std::min(begin + chunk_size, end));
            uint64_t& todo_i = todo_range.first;
            uint64_t& todo_j = todo_range.second;
            while (todo_i != end) {
                ranges.push_back(todo_range);
                todo_i = std::min(todo_i + chunk_size, end);
                todo_j = std::min(todo_j + chunk_size, end);
            }
        }

        // ... compute how many set elements in each range and in total
        std::vector<uint64_t> q_curr_bv_counts; q_curr_bv_counts.resize(ranges.size());
        for (auto& x : q_curr_bv_counts) { x = 0; }
        paryfor::parallel_for<uint64_t>(
                0, ranges.size(), num_threads,
                [&](uint64_t num_range, int tid) {
                    for (uint64_t ii = ranges[num_range].first; ii < ranges[num_range].second; ++ii) {
                        if (q_curr_bv[ii]) {
                            ++q_curr_bv_counts[num_range];
                        }
                    }
                });
        uint64_t q_curr_bv_count = 0;
        for(auto& value : q_curr_bv_counts) { q_curr_bv_count += value; }

        // use a rank support to make a dense mapping from the current bases to an integer range
        // ... pre-allocate default values
        std::vector<uint64_t> q_curr_bv_vec; q_curr_bv_vec.resize(q_curr_bv_count);
        // ... prepare offsets in the vector to fill: we already know the number of items that will come from each range.
        {
            // ...... range 0 elements will be inserted from the position 0
            // ...... range 1 elements will be inserted from the position num_element_range_0
            // ...... range N elements will be inserted from the position num_element_range_0+1+...+N-1
            uint64_t current_starting_pos_for_range = q_curr_bv_count;
            for(int64_t num_range = (int64_t)ranges.size() - 1; num_range >= 0; --num_range) {
                current_starting_pos_for_range -= q_curr_bv_counts[num_range];
                q_curr_bv_counts[num_range] = current_starting_pos_for_range;
            }
        }

        // ... fill in parallel
        paryfor::parallel_for<uint64_t>(
                0, ranges.size(), num_threads,
                [&](uint64_t num_range, int tid) {
                    for (uint64_t ii = ranges[num_range].first; ii < ranges[num_range].second; ++ii) {
                        if (q_curr_bv[ii]) {
                            q_curr_bv_vec[q_curr_bv_counts[num_range]++] = ii;
                        }
                    }
                });

        sdsl::bit_vector q_curr_bv_sdsl(seqidx.seq_length());
        for (auto p : q_curr_bv_vec) {
            q_curr_bv_sdsl[p] = 1;
        }
        sdsl::bit_vector::rank_1_type q_curr_rank;
        sdsl::util::assign(q_curr_rank, sdsl::bit_vector::rank_1_type(&q_curr_bv_sdsl));
        //q_curr_bv_vec.clear();
        // disjoint set structure
        std::vector<DisjointSets::Aint> q_sets_data(q_curr_bv_count);
        // this initializes everything
        auto disjoint_sets = DisjointSets(q_sets_data.data(), q_sets_data.size());

        paryfor::parallel_for<uint64_t>(
            0, ovlp.size(), num_threads, 10000,
            [&](uint64_t k) {
                auto& s = ovlp.at(k);
                auto& r = s.first;
                pos_t p = r.pos;
                for (uint64_t j = r.start; j != r.end; ++j) {
                    // unite both sides of the overlap
                    disjoint_sets.unite(q_curr_rank(j), q_curr_rank(offset(p)));
                    incr_pos(p);
                }
            });
        // now read out our transclosures
        // maps from dset id to query base
        auto* dsets_ptr = new std::vector<std::pair<uint64_t, uint64_t>>(q_curr_bv_count);
        auto& dsets = *dsets_ptr;
        std::pair<uint64_t, uint64_t> max_pair = std::make_pair(std::numeric_limits<uint64_t>::max(), std::numeric_limits<uint64_t>::max());
        paryfor::parallel_for<uint64_t>(
            0, q_curr_bv_count, num_threads, 10000,
            [&](uint64_t j) {
                auto& p = q_curr_bv_vec[j];
                if (!q_seen_bv[p]) {
                    dsets[j] = std::make_pair(disjoint_sets.find(q_curr_rank(p)), p);
                } else {
                    dsets[j] = max_pair;
                }
            });
        //q_curr_bv_vec.clear();
        // remove excluded elements
        dsets.erase(std::remove_if(dsets.begin(), dsets.end(),
                                   [&max_pair](const std::pair<uint64_t, uint64_t>& x) {
                                       return x == max_pair;
                                   }),
                    dsets.end());
        // compress the dsets
        ips4o::parallel::sort(dsets.begin(), dsets.end(), std::less<>(), num_threads);

        uint64_t c = 0;
        assert(dsets.size());
        uint64_t l = dsets.front().first;
        for (auto& d : dsets) {
            if (d.first != l) {
                ++c;
                l = d.first;
            }
            d.first = c;
        }
        /*
        for (auto& d : dsets) {
            std::cerr << "sdset\t" << d.first << "\t" << d.second << std::endl;
        }
        */
        // sort by the smallest starting position in each disjoint set
        std::vector<std::pair<uint64_t, uint64_t>> dsets_by_min_pos(c+1);
        for (uint64_t x = 0; x < c+1; ++x) {
            dsets_by_min_pos[x].second = x;
            dsets_by_min_pos[x].first = std::numeric_limits<uint64_t>::max();
        }
        for (auto& d : dsets) {
            uint64_t& minpos = dsets_by_min_pos[d.first].first;
            minpos = std::min(minpos, d.second);
        }
        ips4o::parallel::sort(dsets_by_min_pos.begin(), dsets_by_min_pos.end(), std::less<>(), num_threads);
        /*
        for (auto& d : dsets_by_min_pos) {
            std::cerr << "sdset_min_pos\t" << d.second << "\t" << d.first << std::endl;
        }
        */
        // invert the naming
        std::vector<uint64_t> dset_names(c+1);
        uint64_t x = 0;
        for (auto& d : dsets_by_min_pos) {
            dset_names[d.second] = x++;
        }
        // rename sdsets and re-sort
        for (auto& d : dsets) {
            d.first = dset_names[d.first];
        }
        ips4o::parallel::sort(dsets.begin(), dsets.end(), std::less<>(), num_threads);
        /*
        for (auto& d : dsets) {
            std::cerr << "sdset_rename\t" << d.first << "\t" << pos_to_string(d.second) << std::endl;
        }
        */
        // now, run the graph emission
        // mark q_seen_bv
        for (auto& d : dsets) {
            const auto& curr_offset = d.second;
            q_seen_bv[curr_offset] = 1;
            ++bases_seen;
        }
        // wait for completion of the last writer
        if (graph_writer != nullptr) {
            graph_writer->join();
            delete graph_writer;
        }
        // spawn the graph writer thread
        graph_writer = new std::thread(write_graph_chunk,
                                       std::ref(seqidx),
                                       std::ref(node_iitree),
                                       std::ref(path_iitree),
                                       std::ref(seq_v_out),
                                       std::ref(range_buffer),
                                       dsets_ptr,
                                       repeat_max,
                                       min_repeat_dist);
    }
    // clean up the last writer
    if (graph_writer != nullptr) {
        graph_writer->join();
        delete graph_writer;
    }
    // close the graph sequence vector
    size_t seq_bytes = seq_v_out.tellp();
    seq_v_out.close();
    flush_ranges(seq_bytes+1, range_buffer, node_iitree, path_iitree);
    assert(range_buffer.empty());
    // close writers
    node_iitree.close_writer();
    path_iitree.close_writer();
    // build node_mm and path_mm indexes
    node_iitree.index(num_threads);
    path_iitree.index(num_threads);
    return seq_bytes;
}


int main(void) {
    std::cout << "Transclosure benchmark:" << std::endl;

    const int NTHREADS = 8;
    const bool KEEP_TEMP = false;

    // temporary file
    auto load_start = std::chrono::system_clock::now();
    char* cwd = get_current_dir_name();
    temp_file::set_dir(std::string(cwd));
    free(cwd);
    temp_file::set_keep_temp(KEEP_TEMP);

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
    const std::string seq_v_file = temp_file::create("seqwish-", ".sqs");
    const std::string node_iitree_idx = temp_file::create("seqwish-", ".sqn");
    const std::string path_iitree_idx = temp_file::create("seqwish-", ".sqp");
    auto node_iitree_ptr = std::make_unique<mmmulti::iitree<uint64_t, seqwish::pos_t>>(node_iitree_idx); // maps graph seq to input seq
    auto& node_iitree = *node_iitree_ptr;
    auto path_iitree_ptr = std::make_unique<mmmulti::iitree<uint64_t, seqwish::pos_t>>(path_iitree_idx); // maps input seq to graph seq
    auto& path_iitree = *path_iitree_ptr;

    auto load_end = std::chrono::system_clock::now();


    std::cout << "Running kernel (" << NTHREADS << " threads)" << std::endl;
    auto kernel_start = std::chrono::system_clock::now();
    size_t graph_length = compute_transitive_closures_kernel(seqidx, aln_iitree, seq_v_file, node_iitree, path_iitree, NTHREADS);
    auto kernel_end = std::chrono::system_clock::now();
    std::cout << "Kernel complete" << std::endl;


    auto write_start = std::chrono::system_clock::now();
    std::cout << graph_length << std::endl;
    // compress
    sdsl::bit_vector seq_id_bv(graph_length+1);
    compact_nodes(seqidx, graph_length, node_iitree, path_iitree, seq_id_bv, NTHREADS);

    sdsl::sd_vector<> seq_id_cbv;
    sdsl::sd_vector<>::rank_1_type seq_id_cbv_rank;
    sdsl::sd_vector<>::select_1_type seq_id_cbv_select;
    sdsl::util::assign(seq_id_cbv, sdsl::sd_vector<>(seq_id_bv));
    seq_id_bv = sdsl::bit_vector(); // clear bitvector
    sdsl::util::assign(seq_id_cbv_rank, sdsl::sd_vector<>::rank_1_type(&seq_id_cbv));
    sdsl::util::assign(seq_id_cbv_select, sdsl::sd_vector<>::select_1_type(&seq_id_cbv));

    // create paths
    const std::string link_mm_idx =  temp_file::create("seqwish-", ".sql");
    auto link_mmset_ptr = std::make_unique<mmmulti::set<std::pair<pos_t, pos_t>>>(link_mm_idx);
    auto& link_mmset = *link_mmset_ptr;
    derive_links(seqidx, node_iitree, path_iitree, seq_id_cbv, seq_id_cbv_rank, seq_id_cbv_select, link_mmset, NTHREADS);

    // emit GFA
    std::ofstream out(GFA_PATH.c_str());
    emit_gfa(out, graph_length, seq_v_file, node_iitree, path_iitree, seq_id_cbv, seq_id_cbv_rank, seq_id_cbv_select, seqidx, link_mmset, NTHREADS);
    std::cout << "Stored graph (" << GFA_PATH << ")" << std::endl;
    auto write_end = std::chrono::system_clock::now();


    auto load_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
            load_end-load_start).count();
    auto kernel_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
            kernel_end-kernel_start).count();
    auto write_time_us = std::chrono::duration_cast<std::chrono::microseconds>(
            write_end-write_start).count();

    std::cout << std::endl;
    std::cout << "load time: " << load_time_us << "us: " << std::endl;
    std::cout << "kernel time: " << kernel_time_us << "us" << std::endl;
    std::cout << "write time: " << write_time_us << "us" << std::endl;
    return 0;
}
