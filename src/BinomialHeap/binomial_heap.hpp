#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <functional>

#include "../base/HeapBase.hpp"

namespace MC {

template< typename Item >
class BinomialHeap : public HeapBase {
protected:
    struct Node;

    struct Node {
        int key;
        Item item;

        Node* parent = nullptr;

        Node* next = nullptr;
        Node* prev = nullptr;

        Node* child = nullptr;

        unsigned degree = 0;

        Node(int k, const Item& i)
            : key(k), item(i) {}

        Node* AddChild(Node* o) {
            ++degree;
            auto c = child;
            o->next = child;
            o->prev = nullptr;
            o->parent = this;
            child = o;
            if (c)
                c->prev = o;
            return this;
        }

        void Reset() {
            parent = child = next = prev = nullptr;
            degree = 0;
        }

        ~Node() {
            for (auto c = child; c; c = c->next)
                delete c;
        }

    };

    Node* min = nullptr;
    std::size_t count = 0;
    std::size_t rootCount = 0;

public:

    using NodeType = BinomialHeap::Node;

    BinomialHeap() : HeapBase("Binomial heap") {}

    bool Empty() const {
        return min == nullptr;
    }

    const Node& Min() const {
        if (!min)
            EmptyException();
        return *min;
    }

    const Node* Insert(int k, const Item& i) {
        auto n = new Node(k, i);
        ++count;
        AddToRoots(n);
        Consolidate();
        return n;
    }

    void DecreaseKey(const Node* node, int k) {
        auto* n = const_cast< Node* >(node);
        if (k > n->key)
            InvalidKeyException();
        n->key = k;
        BubbleUp(n);
        Consolidate();
        if (n->key < min->key)
            min = n;
    }

    std::unique_ptr< Node > ExtractMin() {
        if (!min)
            EmptyException();
        auto m = min;
        std::unique_ptr< Node > ret(m);
        --count;
        RemoveFromRoot(min);
        PromoteChildren(m);
        if (min)
            Consolidate();
        ret->Reset();
        return ret;
    }

    ~BinomialHeap() {
        for (auto p = min; p != nullptr; p = p->next)
            delete p;
    }

protected:
    void Consolidate() {
        unsigned bound = std::ceil(std::log2(count)) + 1;
        std::vector< Node* > roots(bound);

        for (auto p = min; p;) {
            --rootCount;
            auto n = p->next;
            auto d = p->degree;
            while (roots[d]) {
                auto q = roots[d];
                roots[d] = nullptr;
                if (q->key < p->key)
                    std::swap(p, q);

                p->AddChild(q);
                d = p->degree;
            }
            roots[d] = p;
            p = n;
        }

        min = nullptr;
        for (auto p : roots) {
            AddToRoots(p);
        }
    }

    void RemoveFromRoot(Node* n) {
        --rootCount;
        min = n->next;
        if (n->next)
            n->prev = nullptr;
    }

    void PromoteChildren(Node* n) {
        for (auto c = n->child; c;) {
            auto next = c->next;
            AddToRoots(c);
            c = next;
        }
    }

    void BubbleUp(Node* n) {
        while (n && n->parent && n->key < n->parent->key)
            n = SwapNodes(n->parent, n);
    }

    Node* SwapNodes(Node* up, Node* lo) {
        auto upp = up->prev;
        auto upn = up->next;
        auto lop = lo->prev;
        auto lon = lo->next;

        std::swap(up->prev, lo->prev);
        std::swap(up->next, lo->next);
        lo->parent = up->parent;
        up->parent = lo;
        std::swap(up->child, lo->child);
        std::swap(up->degree, lo->degree);

        if (lo->child == lo)
            lo->child = up;

        if (lo->parent && lo->parent->child == up)
            lo->parent->child = lo;

        if (upp)
            upp->next = lo;
        if (upn)
            upn->prev = lo;

        if (lop)
            lop->next = up;
        if (lon)
            lon->prev = up;

        for (;lon; lon = lon->next)
            lon->parent = lo;

        return lo;
    }

    void AddToRoots(Node* n) {
        if (!n)
            return;

        n->next = n->prev = n->parent = nullptr;

        ++rootCount;
        if (!min) {
            min = n;
            return;
        }

        if (n->key < min->key) { // insert to 1st position
            n->next = min;
            min->prev = n;
            min = n;
        } else { // insert 2nd
            auto x = min->next;
            n->prev = min;
            n->next = x;
            min->next = n;
            if (x)
                x->prev = n;
        }

    }
};
}

