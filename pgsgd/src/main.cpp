#include <iostream>

#include <omp.h>
#include <vector>
#include <atomic>
#include <random>

#include "algorithms/draw.hpp"
#include "algorithms/layout.hpp"        // needs to be included before utils.hpp/odgi.hpp
#include "odgi.hpp"
#include "utils.hpp"
#include "weakly_connected_components.hpp"

#include "cuda/layout.h"
#include "dirty_zipfian_int_distribution.h"


const int NTHREADS = 6;
const int ITER_MAX = 30;


void layout_kernel(cuda::layout_config_t config, double *etas, double *zetas, cuda::node_data_t &node_data, cuda::path_data_t &path_data){
    int nbr_threads = config.nthreads;
    std::cout << "cuda cpu layout (" << nbr_threads << " threads)" << std::endl;
    std::vector<uint64_t> path_dist;
    for (int p = 0; p < path_data.path_count; p++) {
        path_dist.push_back(uint64_t(path_data.paths[p].step_count));
    }

#pragma omp parallel num_threads(nbr_threads)
    {
        int tid = omp_get_thread_num();

        XoshiroCpp::Xoshiro256Plus gen(9399220 + tid);
        std::uniform_int_distribution<uint64_t> flip(0, 1);
        std::discrete_distribution<> rand_path(path_dist.begin(), path_dist.end());

        const int steps_per_thread = config.min_term_updates / nbr_threads;

//#define profiling
#ifdef profiling
        auto total_duration_dist = std::chrono::duration<double>::zero(); // total time on computing distance: in seconds
        auto total_duration_sgd = std::chrono::duration<double>::zero(); // total time on SGD: in seconds
        // detailed analysis on different parts of Updating Coordinates Part
        auto total_duration_compute_first = std::chrono::duration<double>::zero();
        auto total_duration_load = std::chrono::duration<double>::zero();
        auto total_duration_compute_second = std::chrono::duration<double>::zero();
        auto total_duration_store = std::chrono::duration<double>::zero();
        // detailed analysis on different parts of Getting Distance Part
        auto total_duration_one_step_gen = std::chrono::duration<double>::zero();
        auto total_duration_two_step_gen = std::chrono::duration<double>::zero();
        auto total_duration_get_distance = std::chrono::duration<double>::zero();


        std::chrono::high_resolution_clock::time_point start_dist;
        std::chrono::high_resolution_clock::time_point end_dist;
        std::chrono::high_resolution_clock::time_point start_sgd;
        std::chrono::high_resolution_clock::time_point one_step_gen;
        std::chrono::high_resolution_clock::time_point two_step_gen;

        // detailed analysis on Updating Coordinates part
        std::chrono::high_resolution_clock::time_point before_load;
        std::chrono::high_resolution_clock::time_point after_load;
        std::chrono::high_resolution_clock::time_point before_store;
        std::chrono::high_resolution_clock::time_point after_store;
#endif

        for (int iter = 0; iter < config.iter_max; iter++ ) {
            // synchronize all threads before each iteration
#pragma omp barrier
            for (int step = 0; step < steps_per_thread; step++ ) {
#ifdef profiling
                start_dist = std::chrono::high_resolution_clock::now();
#endif
                // get path
                uint32_t path_idx = rand_path(gen);
                cuda::path_t p = path_data.paths[path_idx];
                if (p.step_count < 2) {
                    continue;
                }

                std::uniform_int_distribution<uint32_t> rand_step(0, p.step_count-1);

                uint32_t s1_idx = rand_step(gen);
#ifdef profiling
                one_step_gen = std::chrono::high_resolution_clock::now();
                total_duration_one_step_gen += std::chrono::duration_cast<std::chrono::nanoseconds>(one_step_gen - start_dist);
#endif
                uint32_t s2_idx;
                if (iter + 1 >= config.first_cooling_iteration || flip(gen)) {
                    if (s1_idx > 0 && flip(gen) || s1_idx == p.step_count-1) {
                        // go backward
                        uint32_t jump_space = std::min(config.space, s1_idx);
                        uint32_t space = jump_space;
                        if (jump_space > config.space_max) {
                            space = config.space_max + (jump_space - config.space_max) / config.space_quantization_step + 1;
                        }
                        dirtyzipf::dirty_zipfian_int_distribution<uint64_t>::param_type z_p(1, jump_space, config.theta, zetas[space]);
                        dirtyzipf::dirty_zipfian_int_distribution<uint64_t> z(z_p);
                        uint32_t z_i = (uint32_t) z(gen);
                        s2_idx = s1_idx - z_i;
                    } else {
                        // go forward
                        uint32_t jump_space = std::min(config.space, p.step_count - s1_idx - 1);
                        uint32_t space = jump_space;
                        if (jump_space > config.space_max) {
                            space = config.space_max + (jump_space - config.space_max) / config.space_quantization_step + 1;
                        }
                        dirtyzipf::dirty_zipfian_int_distribution<uint64_t>::param_type z_p(1, jump_space, config.theta, zetas[space]);
                        dirtyzipf::dirty_zipfian_int_distribution<uint64_t> z(z_p);
                        uint32_t z_i = (uint32_t) z(gen);
                        s2_idx = s1_idx + z_i;
                    }
                } else {
                    do {
                        s2_idx = rand_step(gen);
                    } while (s1_idx == s2_idx);
                }
#ifdef profiling
                two_step_gen = std::chrono::high_resolution_clock::now();
                total_duration_two_step_gen += std::chrono::duration_cast<std::chrono::nanoseconds>(two_step_gen - one_step_gen);
#endif
                assert(s1_idx < p.step_count);
                assert(s2_idx < p.step_count);

                uint32_t n1_id = p.elements[s1_idx].node_id;
                int64_t n1_pos_in_path = p.elements[s1_idx].pos;
                bool n1_is_rev = (n1_pos_in_path < 0)? true: false;
                n1_pos_in_path = std::abs(n1_pos_in_path);

                uint32_t n2_id = p.elements[s2_idx].node_id;
                int64_t n2_pos_in_path = p.elements[s2_idx].pos;
                bool n2_is_rev = (n2_pos_in_path < 0)? true: false;
                n2_pos_in_path = std::abs(n2_pos_in_path);

                uint32_t n1_seq_length = node_data.nodes[n1_id].seq_length;
                bool n1_use_other_end = flip(gen);
                if (n1_use_other_end) {
                    n1_pos_in_path += uint64_t{n1_seq_length};
                    n1_use_other_end = !n1_is_rev;
                } else {
                    n1_use_other_end = n1_is_rev;
                }

                uint32_t n2_seq_length = node_data.nodes[n2_id].seq_length;
                bool n2_use_other_end = flip(gen);
                if (n2_use_other_end) {
                    n2_pos_in_path += uint64_t{n2_seq_length};
                    n2_use_other_end = !n2_is_rev;
                } else {
                    n2_use_other_end = n2_is_rev;
                }

                double term_dist = std::abs(static_cast<double>(n1_pos_in_path) - static_cast<double>(n2_pos_in_path));

                if (term_dist < 1e-9) {
                    term_dist = 1e-9;
                }
#ifdef profiling
                end_dist = std::chrono::high_resolution_clock::now();
                total_duration_get_distance += std::chrono::duration_cast<std::chrono::nanoseconds>(end_dist - two_step_gen);

                total_duration_dist += std::chrono::duration_cast<std::chrono::nanoseconds>(end_dist - start_dist);

                start_sgd = std::chrono::high_resolution_clock::now();
#endif

                double w_ij = 1.0 / term_dist;

                double mu = etas[iter] * w_ij;
                if (mu > 1.0) {
                    mu = 1.0;
                }

                double d_ij = term_dist;

                int n1_offset = n1_use_other_end? 2: 0;
                int n2_offset = n2_use_other_end? 2: 0;

#ifdef profiling
                before_load = std::chrono::high_resolution_clock::now();
                total_duration_compute_first += std::chrono::duration_cast<std::chrono::nanoseconds>(before_load - start_sgd);
#endif
                std::atomic<float> *x1 = &node_data.nodes[n1_id].coords[n1_offset];
                std::atomic<float> *x2 = &node_data.nodes[n2_id].coords[n2_offset];
                std::atomic<float> *y1 = &node_data.nodes[n1_id].coords[n1_offset + 1];
                std::atomic<float> *y2 = &node_data.nodes[n2_id].coords[n2_offset + 1];

                double dx = float(x1->load() - x2->load());
                double dy = float(y1->load() - y2->load());
#ifdef profiling
                after_load = std::chrono::high_resolution_clock::now();
                total_duration_load += std::chrono::duration_cast<std::chrono::nanoseconds>(after_load - before_load);
#endif
                if (dx == 0.0) {
                    dx = 1e-9;
                }

                double mag = sqrt(dx * dx + dy * dy);
                double delta = mu * (mag - d_ij) / 2.0;
                //double delta_abs = std::abs(delta);

                double r = delta / mag;
                double r_x = r * dx;
                double r_y = r * dy;

#ifdef profiling
                before_store = std::chrono::high_resolution_clock::now();
                total_duration_compute_second += std::chrono::duration_cast<std::chrono::nanoseconds>(before_store - after_load);
#endif
                x1->store(x1->load() - float(r_x));
                y1->store(y1->load() - float(r_y));
                x2->store(x2->load() + float(r_x));
                y2->store(y2->load() + float(r_y));
#ifdef profiling
                after_store = std::chrono::high_resolution_clock::now();
                total_duration_store += std::chrono::duration_cast<std::chrono::nanoseconds>(after_store - before_store);
                total_duration_sgd += std::chrono::duration_cast<std::chrono::nanoseconds>(after_store - start_sgd);
#endif
            }
        }

#ifdef profiling
        std::stringstream msg;
        msg << "Thread[" << tid << "]: Dataloading time = " << total_duration_dist.count() << " sec;\t" << "Compute time = " << total_duration_sgd.count() << " sec." << std::endl;

        msg << std::left
            << std::setw(40) << "Getting Distance Part Breakdown: " << std::endl
            << std::setw(20) << "[0] One Step Gen: "
            << std::setw(10) << total_duration_one_step_gen.count()
            << std::setw(10)  << " sec;"
            << std::setw(20) << "[1] Two Steps Gen: "
            << std::setw(10) << total_duration_two_step_gen.count()
            << std::setw(10)  << " sec;"
            << std::setw(20) << "[2] Get Distance: "
            << std::setw(10) << total_duration_get_distance.count()
            << std::setw(10) << " sec."
            << std::endl;

        msg << std::setw(40) << "Updating Coordinate Part Breakdown: " << std::endl
            << std::setw(20) << "[0] First Compute: "
            << std::setw(10) << total_duration_compute_first.count()
            << std::setw(10)  << " sec;"
            << std::setw(20) << "[1] Load Pos: "
            << std::setw(10) << total_duration_load.count()
            << std::setw(10)  << " sec;"
            << std::setw(20) << "[2] Second Compute: "
            << std::setw(10) << total_duration_compute_second.count()
            << std::setw(10)  << " sec;"
            << std::setw(20) << "[3] Update Pos: "
            << std::setw(10) << total_duration_store.count()
            << std::setw(10)  << " sec."
            << std::endl << std::endl;

        std::cerr << msg.str();
#endif
    }
}


int main() {
    std::cout << "pgsgd benchmark:" << std::endl;

    odgi::graph_t graph;
    utils::handle_gfa_odgi_input("data/DRB1-3123.og", "layout", false, NTHREADS, graph);
    // utils::handle_gfa_odgi_input("data/chr20.og", "layout", false, NTHREADS, graph);


    // create random X and Y coordinates
    std::vector<double> X(graph.get_node_count() * 2);
    std::vector<double> Y(graph.get_node_count() * 2);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::normal_distribution<double> gaussian_noise(0,  sqrt(graph.get_node_count() * 2));
    uint64_t len = 0;
    graph.for_each_handle([&](const handle_t &h) {
          uint64_t pos = 2 * number_bool_packing::unpack_number(h);
          X[pos] = len;
          Y[pos] = gaussian_noise(rng);
          len += graph.get_length(h);
          X[pos + 1] = len;
          Y[pos + 1] = gaussian_noise(rng);
      });


    // create node data structure
    // consisting of sequence length and coords
    uint32_t node_count = graph.get_node_count();
    std::cout << "node_count: " << node_count << std::endl;
    assert(graph.min_node_id() == 1);
    assert(graph.max_node_id() == node_count);
    assert(graph.max_node_id() - graph.min_node_id() + 1 == node_count);

    cuda::node_data_t node_data;
    node_data.node_count = node_count;
    node_data.nodes = new cuda::node_t[node_count];
    for (int node_idx = 0; node_idx < node_count; node_idx++) {
        //assert(graph.has_node(node_idx));
        cuda::node_t *n_tmp = &node_data.nodes[node_idx];

        // sequence length
        const handlegraph::handle_t h = graph.get_handle(node_idx + 1, false);
        // NOTE: unable store orientation (reverse), since this information is path dependent
        n_tmp->seq_length = graph.get_length(h);

        // copy random coordinates
        n_tmp->coords[0].store(float(X[node_idx * 2]));
        n_tmp->coords[1].store(float(Y[node_idx * 2]));
        n_tmp->coords[2].store(float(X[node_idx * 2 + 1]));
        n_tmp->coords[3].store(float(Y[node_idx * 2 + 1]));
    }


    // create path data structure
    uint32_t path_count = graph.get_path_count();
    // TODO check correct value of max_path_step_count and sum_path_step_count with chr20
    uint64_t max_path_step_count = 0;
    uint64_t sum_path_step_count = 0;
    cuda::path_data_t path_data;
    path_data.path_count = path_count;
    path_data.total_path_steps = 0;
    path_data.paths = new cuda::path_t[path_count];

    vector<odgi::path_handle_t> path_handles{};
    path_handles.reserve(path_count);
    graph.for_each_path_handle(
        [&] (const odgi::path_handle_t& p) {
            path_handles.push_back(p);
            path_data.total_path_steps += graph.get_step_count(p);
        });
    path_data.element_array = new cuda::path_element_t[path_data.total_path_steps];

    // get length and starting position of all paths
    uint32_t first_step_counter = 0;
    for (int path_idx = 0; path_idx < path_count; path_idx++) {
        odgi::path_handle_t p = path_handles[path_idx];
        int step_count = graph.get_step_count(p);
        sum_path_step_count += step_count;
        max_path_step_count = std::max(max_path_step_count, uint64_t(step_count));
        path_data.paths[path_idx].step_count = step_count;
        path_data.paths[path_idx].first_step_in_path = first_step_counter;
        first_step_counter += step_count;
    }

    std::cout << "sum_path_step_count: " << sum_path_step_count << std::endl;
    std::cout << "max_path_step_count: " << max_path_step_count << std::endl;

#pragma omp parallel for num_threads(NTHREADS)
    for (int path_idx = 0; path_idx < path_count; path_idx++) {
        odgi::path_handle_t p = path_handles[path_idx];
        //std::cout << graph.get_path_name(p) << ": " << graph.get_step_count(p) << std::endl;

        uint32_t step_count = path_data.paths[path_idx].step_count;
        uint32_t first_step_in_path = path_data.paths[path_idx].first_step_in_path;
        if (step_count == 0) {
            path_data.paths[path_idx].elements = NULL;
        } else {
            cuda::path_element_t *cur_path = &path_data.element_array[first_step_in_path];
            path_data.paths[path_idx].elements = cur_path;

            odgi::step_handle_t s = graph.path_begin(p);
            int64_t pos = 1;
            // Iterate through path
            for (int step_idx = 0; step_idx < step_count; step_idx++) {
                odgi::handle_t h = graph.get_handle_of_step(s);
                //std::cout << graph.get_id(h) << std::endl;

                cur_path[step_idx].node_id = graph.get_id(h) - 1;
                cur_path[step_idx].pidx = uint32_t(path_idx);
                // store position negative when handle reverse
                if (graph.get_is_reverse(h)) {
                    cur_path[step_idx].pos = -pos;
                } else {
                    cur_path[step_idx].pos = pos;
                }
                pos += graph.get_length(h);

                // get next step
                if (graph.has_next_step(s)) {
                    s = graph.get_next_step(s);
                } else if (!(step_idx == step_count-1)) {
                    // should never be reached
                    std::cout << "Error: Here should be another step" << std::endl;
                }
            }
        }
    }


    cuda::layout_config_t config;
    config.iter_max = ITER_MAX;
    config.min_term_updates = 10.0 * sum_path_step_count;
    config.eta_max = max_path_step_count * max_path_step_count;    // max learning rate: square longest path
    config.eps = 0.01;                                              // final learning rate
    config.iter_with_max_learning_rate = 0;
    config.first_cooling_iteration = std::floor(0.5 * double{ITER_MAX});
    config.theta = 0.99;            // theta value for zipf distribution
    config.space = max_path_step_count;              // maximum space size of zipf distribution
    config.space_max = 1000;                            // maximum space size of zipf distribution after which quantization occurs
    config.space_quantization_step = 100;               // size of quantization step of zipf distribution
    config.nthreads = NTHREADS;


    // precompute learning rate
    double *etas = new double[config.iter_max];
    const double eta_min = config.eps / 1.0;
    const double lambda = log(config.eta_max / eta_min) / ((double) config.iter_max - 1);
    for (int32_t i = 0; i < config.iter_max; i++) {
        double eta = config.eta_max * exp(-lambda * (std::abs(i - config.iter_with_max_learning_rate)));
        etas[i] = isnan(eta)? eta_min : eta;
    }


    // cache zipf zetas
    double *zetas;
    uint64_t zetas_cnt = ((config.space <= config.space_max)? config.space : (config.space_max + (config.space - config.space_max) / config.space_quantization_step + 1)) + 1;
    std::cout << "zetas_cnt: " << zetas_cnt << std::endl;
    std::cout << "space_max: " << config.space_max << std::endl;
    std::cout << "config.space: " << config.space << std::endl;
    std::cout << "config.space_quantization: " << config.space_quantization_step << std::endl;

    zetas = new double[zetas_cnt];
    double zeta_tmp = 0.0;
    for (uint64_t i = 1; i < config.space + 1; i++) {
        zeta_tmp += dirtyzipf::fast_precise_pow(1.0 / i, config.theta);
        if (i <= config.space_max) {
            zetas[i] = zeta_tmp;
        }
        if (i >= config.space_max && (i - config.space_max) % config.space_quantization_step == 0) {
            zetas[config.space_max + 1 + (i - config.space_max) / config.space_quantization_step] = zeta_tmp;
        }
    }


    // run kernel
    layout_kernel(config, etas, zetas, node_data, path_data);


    // copy coords back to X, Y vectors
    for (int node_idx = 0; node_idx < node_count; node_idx++) {
        cuda::node_t *n = &(node_data.nodes[node_idx]);
        // coords[0], coords[1], coords[2], coords[3] are stored consecutively.
        std::atomic<float> *coords = n->coords;
        // check if coordinates valid (not NaN or infinite)
        for (int i = 0; i < 4; i++) {
            if (!isfinite(coords[i].load())) {
                std::cout << "WARNING: invalid coordiate" << std::endl;
            }
        }
        X[node_idx * 2] = double(coords[0].load());
        Y[node_idx * 2] = double(coords[1].load());
        X[node_idx * 2 + 1] = double(coords[2].load());
        Y[node_idx * 2 + 1] = double(coords[3].load());
        //std::cout << "coords of " << node_idx << ": [" << X[node_idx*2] << "; " << Y[node_idx*2] << "] ; [" << X[node_idx*2+1] << "; " << Y[node_idx*2+1] <<"]\n";
    }


    free(etas);
    free(node_data.nodes);
    free(path_data.paths);
    free(path_data.element_array);
    free(zetas);


    // refine order by weakly connected components
    std::vector<std::vector<handlegraph::handle_t>> weak_components = odgi::algorithms::weakly_connected_component_vectors(&graph);

    double border = 1000.0;
    double curr_y_offset = border;
    std::vector<odgi::algorithms::coord_range_2d_t> component_ranges;
    for (auto& component : weak_components) {
        component_ranges.emplace_back();
        auto& component_range = component_ranges.back();
        for (auto& handle : component) {
            uint64_t pos = 2 * number_bool_packing::unpack_number(handle);
            for (uint64_t j = pos; j <= pos+1; ++j) {
                component_range.include(X[j], Y[j]);
            }
        }
        component_range.x_offset = component_range.min_x - border;
        component_range.y_offset = curr_y_offset - component_range.min_y;
        curr_y_offset += component_range.height() + border;
    }

    for (uint64_t num_component = 0; num_component < weak_components.size(); ++num_component) {
        auto& component_range = component_ranges[num_component];

        for (auto& handle :  weak_components[num_component]) {
            uint64_t pos = 2 * number_bool_packing::unpack_number(handle);

            for (uint64_t j = pos; j <= pos+1; ++j) {
                X[j] -= component_range.x_offset;
                Y[j] += component_range.y_offset;
            }
        }
    }


    // generate layout file
    odgi::algorithms::layout::Layout lay(X, Y);
    ofstream f("out.lay");
    lay.serialize(f);
    f.close();
}
