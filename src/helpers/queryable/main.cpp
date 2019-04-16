#include <iostream>
#include "queryable2.hpp"

using namespace randoms;

int main() {
    std::vector< int > v = {0, 1, 2, 3, 4, 5};
    auto q = as_queryable(v)
        .where([](int i){ return i % 2 == 0; });

    for (auto i : q) {
        std::cout << i << " ";
    }
}