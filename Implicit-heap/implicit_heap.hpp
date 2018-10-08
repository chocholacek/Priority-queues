#pragma once

#include <memory>

#include "../Explicit-heap/explicit_heap.hpp"

namespace PriorityQueues {
template< typename T, std::size_t ary, typename Compare >
class implicit_heap {
    using ptr = std::unique_ptr< T >;

    struct ValueCompare {
        bool operator()(const ptr& lhs, const ptr& rhs) {
            return Compare()(*lhs, *rhs);
        }
    };

    using heap = explicit_heap< ptr, ary, ValueCompare >;

    heap h;

public:
    template < typename... Args >
    void insert(Args&&... args) {
        h.insert(std::make_unique< T >(std::forward< Args >(args)...));
    }

    T extract() {
        return *h.extract();
    }
};
}