#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <forward_list>

#include "../base/HeapBase.hpp"

namespace MC {

template< typename Item >
class BinomialHeap : HeapBase {
protected:
    struct Node;

    using unique = std::unique_ptr< Node >;

    struct Node {
        int key;
        Item item;

        Node* parent = nullptr;

        std::vector< unique > children;

        unsigned degree = 0;

        Node(int k, const Item& i)
            : key(k), item(i) {}

        void addChild(unique&& c) {
            ++degree;
            c->parent = this;
            children.push_back(std::move(c));
        }

        void swap(Node* n) {
            std::swap(key, n->key);
            std::swap(item, n->item);
        }

    };


    std::array< unique, 64 > roots;

    Node* min = nullptr;

    int minIndex = -1;

public:

    const Node& Min() const {
        if (!min)
            EmptyException();
        return *min;
    }

    void Insert(int k, const Item& i) {
        auto n = std::make_unique< Node >(k, i);

        if (!min || k < min->key)
            min = n.get();

        attemptInsert(std::move(n));
    }

    void DecreaseKey(const Node& node, int k) {
        auto* n = const_cast< Node* >(&node);
        if (k > n->key)
            InvalidKeyException();

        n->key = k;
        bubbleUp(n);
    }

    Item ExtractMin() {
        if (!min)
            EmptyException();

        Item i = min->item;
        promoteChildren(min);
        popMin();
        assignNewMin();
        return i;
    }

protected:
    unique merge(unique&& a, unique&& b) {
        if (b->key < a->key)
            std::swap(a, b);

        a->addChild(std::move(b));
        return std::move(a);
    }

    void attemptInsert(unique&& ptr) {
        // degree is not occupied
        if (!roots[ptr->degree]) {
            roots[ptr->degree] = std::move(ptr);
            return;
        }

        // degree occupied, merge needed
        auto m = merge(std::move(ptr), std::move(roots[ptr->degree]));
        // recursively call on higher degree
        attemptInsert(std::move(m));
    }

    void bubbleUp(Node *n) {
        // n == null || parent == null
        if (!n || !n->parent)
            return;

        if (n->key < n->parent->key) {
            n->swap(n->parent);
            bubbleUp(n->parent);
        }
    }

    void promoteChildren(Node* n) {
        auto& children = n->children;
        while (!children.empty()) {
            children.back()->parent = nullptr;
            attemptInsert(std::move(children.back()));
            children.pop_back();
        }
    }

    void popMin() {
        auto it = std::find_if(
                roots.begin(),
                roots.end(),
                [this](const auto& p) { return p.get() == this->min; }
        );

        if (it != roots.end()) {
            it->reset();
        }
    }

    void assignNewMin() {
        min = nullptr;
        Node* m = min;

        for (auto& p : roots) {
            if (!p)
                continue;

            if (!m || p->key < m->key)
                m = p.get();
        }

        min = m;
    }
};
}

