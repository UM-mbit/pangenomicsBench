#include <iostream>

#include "cuda/layout.h"

#include "odgi.hpp"
#include "utils.hpp"

const int num_threads = 6;


int main() {
    std::cout << "pgsgd benchmark:" << std::endl;

    odgi::graph_t graph;
    utils::handle_gfa_odgi_input("data/DRB1-3123.og", "layout", false, num_threads, graph);
}
