#pragma once

#include <list>
#include <memory>
#include <vector>
#include "circular_list.hpp"


namespace PriorityQueues {
template < typename T >
class fibonacci_heap {

    using list = circular_list< T >;
    using node = list::node;
    using shared_ptr = std::shared_ptr< node >;
    using weak_ptr = std::weak_ptr< node >;


    shared_ptr root;
    list nodes;

public:
    template < typename... Args >
    void insert(Args&&... args) {
        nodes.insert(std::forward< Args >(args)...);
        if (!root) {
            root = nodes.root;
            return;
        }
    }

    T extract() {
        if (!root)
            throw;
        T ret = std::move(root->value);

        if (root->child) {

        }
        return ret;
    }

private:
    template < typename F >
    void foreach(shared_ptr n, F f) const {
        if (!n)
            return;
        if (n->right == n) { // only one
            // TODO
            return;
        }
        shared_ptr end = n;
        f(n);
        n = n->right;
        while (n != end) {
            f(n);
            n = n->right;
        }
    }
};
}