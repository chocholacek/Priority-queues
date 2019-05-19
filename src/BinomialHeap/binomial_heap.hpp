#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <functional>
#include <list>
#include "../base/HeapBase.hpp"

namespace MC {

template<typename Item>
class BinomialHeap : public HeapBase {
    struct Node {
        int key;
        Item item;

        Node* parent = nullptr;

        std::list< Node* > children;

        Node(int k, const Item& i) : key(k), item(i) {
        }

        void AddChild(Node* n) {
            n->parent = this;
            children.push_back(n);
        }

        auto FindChild(Node* n) {
            return std::find(children.begin(), children.end(), n);
        }

        int Degree() const { return children.size(); }

        ~Node() {
            for (auto p : children)
                delete p;
        }
    };

    std::list< Node* > roots;
    int count = 0;

    const Node* MinImpl() const {
        return *std::min_element(roots.begin(), roots.end(), [](const auto& x, const auto& y) {
            return x->key < y->key;
        });
    }

public:
    using NodeType = Node;

    BinomialHeap() : HeapBase("binomial heap") {}

    bool Empty() const { return roots.empty(); }

    const Node& Min() const {
        if (Empty())
            EmptyException();
        return *MinImpl();
    }

    const Node* Insert(int key, const Item& item) {
        auto n = new Node(key, item);
        roots.push_back(n);
        ++count;
        if (count >= 1)
            Consolidate();

        return n;
    }

    void DecreaseKey(const Node* node, int key) {
        if (key > node->key)
            InvalidKeyException();

        auto x = const_cast< Node* >(node);
        x->key = key;

        while (x->parent && x->key < x->parent->key) {
            Swap(x->parent, x);
        }
    }

    std::unique_ptr< Node > ExtractMin() {
        if (Empty())
            EmptyException();

        auto min = const_cast< Node* >(MinImpl());

        std::unique_ptr< Node > ret(min);
        auto it = std::find(roots.begin(), roots.end(), min);
        roots.erase(it);
        --count;

        auto cn = std::move(min->children);
        for (auto c : cn) {
            c->parent = nullptr;
            roots.push_back(c);
        }
        if (roots.empty())
            min = nullptr;
        else
            Consolidate();
        return ret;
    }

    ~BinomialHeap() {
        for (auto p : roots)
            delete p;
    }

private:



    void Swap(Node* x, Node* y) {
        auto cxi = x->FindChild(y);

        y->parent = x->parent;
        if (y->parent)
            *y->parent->FindChild(x) = y;
        else {
            auto it = std::find(roots.begin(), roots.end(), x);
            *it = y;
        }
        x->parent = y;
        *cxi = x;
        std::for_each(x->children.begin(), x->children.end(), [y](auto p) {p->parent = y; });
        std::for_each(y->children.begin(), y->children.end(), [x](auto p) {p->parent = x; });
        std::swap(x->children, y->children);
    }

    void Consolidate() {
        int bound = std::ceil(std::log2(count)) + 1;
        std::vector< Node* > tmp(bound, nullptr);
        for (auto x : roots) {
            auto d = x->Degree();
            while (tmp[d]) {
                auto y = tmp[d];

                if (y->key < x->key)
                    std::swap(x, y);

                x->AddChild(y);
                tmp[d] = nullptr;
                ++d;
            }

            tmp[d] = x;
        }

        roots.clear();

        for (auto p : tmp) {
            if (!p)
                continue;
            roots.push_back(p);
        }
    }
};
}

