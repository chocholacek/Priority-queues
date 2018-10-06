#pragma once

#include <cstdlib>
#include <vector>

/**
 * Implementation of Max Heap
 */

namespace PriorityQueues {
template< typename T, std::size_t ary, typename Compare >
class explicit_heap {
    std::vector< T > _data;

    std::size_t parent(std::size_t index) { return (index - 1) / ary; }

    std::size_t min(std::size_t index) {
        std::size_t begin = ary * index + 1,
                end = ary * index + ary,
                smallest = index;

        if (end >= _data.size())
            end = _data.size() - 1;
        for (;begin <= end; ++begin) {
            if (Compare()(_data[begin], _data[smallest])) {
                smallest = begin;
            }
        }
        return smallest;
    }

    void heapify(std::size_t index) noexcept {
        if (index >= _data.size())
            return;

        std::size_t smallest = min(index);

        if (smallest != index) {
            std::swap(_data[index], _data[smallest]);
            heapify(smallest);
        }
    }

    void decrease_key() {
        std::size_t i = _data.size() - 1;
        while (i > 0 && Compare()(_data[i], _data[parent(i)])) {
            std::swap(_data[parent(i)], _data[i]);
            i = parent(i);
        }
    }

public:

    template < typename... Args >
    void insert(Args&&... args) {
        _data.emplace_back(std::forward< Args >(args)...);
        decrease_key();
    }

    T extract_min() {
        std::swap(_data.front(), _data.back());
        T min = std::move(_data.back());
        _data.pop_back();
        heapify(0);
        return min;
    }

    bool operator==(const std::vector< int >& v) {
        return _data == v;
    }

};
}