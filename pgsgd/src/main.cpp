#include <iostream>
#include "cuda/layout.h"

int main() {
    std::cout << "hello, world" << std::endl;
    cuda::node_t tmp;
    tmp.seq_length = 1024;
    std::cout << tmp.seq_length << std::endl;
}
