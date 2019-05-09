#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <functional>

#include "../FibonacciHeap/fibonacci_heap.hpp"

namespace MC {

template<typename Item>
class BinomialHeap : protected FibonacciHeap<Item> {
    using Base = FibonacciHeap<Item>;

public:
    using Base::Min;
    using Base::Empty;
    using Base::DecreaseKey;
    using Base::ExtractMin;
    using Base::Name;
    using Base::Infinity;
    using NodeType = typename Base::NodeType;

    BinomialHeap() : Base("BinomialHeap") {}

    const NodeType* Insert(int key, const Item& i) {
        auto r = Base::Insert(key, i);
        Base::_consolidate();
        return r;
    }
};
}

