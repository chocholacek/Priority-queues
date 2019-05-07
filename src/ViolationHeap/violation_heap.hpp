#pragma once

#include <cmath>
#include <functional>
#include <memory>
#include <vector>
#include "../base/HeapBase.hpp"

namespace MC {

template<typename Item>
class ViolationHeap : public HeapBase {
protected:
    struct Node {
        friend ViolationHeap;

        int key;
        Item item;

        Node* next = nullptr;
        Node* prev = nullptr;

        Node* child = nullptr;

        int rank = 0;

        Node(int k, const Item& i) : key(k), item(i) {}

        void AddChild(Node* n) {
            auto c = child;
            n->prev = this;
            n->next = child;
            child = n;

            if (c)
                c->prev = n;
        }

        Node* ActiveChildWithLargetsRank() const {
            if (!child)
                return nullptr;

            int r1 = child->rank;
            if (!child->next)
                return child;
            int r2 = child->next->rank;

            return r1 > r2 ? child : child->next;
        }

        bool IsRoot() const {
            return prev == nullptr;
        }

        // returns pointer to parent if node is active
        static Node* IsActive(Node* which) {
            if (which->IsRoot())
                return nullptr;
            if (which == which->prev->child)
                return which->prev;
            if (which == which->prev->prev->child)
                return which->prev->prev;
            return nullptr;
        }

        void Replace(Node* by) {
            auto byp = by->prev;
            auto byn = by->next;

            by->next = next;
            by->prev = prev;
            if (next)
                next->prev = by;
            if (this == prev->child) { // this is 1st child
                prev->child = by;
            } else {
                prev->next = by;
            }

            if (child == by) { // by is first child
                child = byn;
                if (byn)
                    byn->prev = this;
            } else {
                child->next = byn;
                if (child->next)
                    child->next->prev = byp;
            }
        }

        // returns old rank
        int RecalculateRank() {
            int o = rank;
            int r1 = child ? child->rank : -1;
            int r2 = -1;
            if (child && child->next)
                r2 = child->next->rank;
            rank = std::ceil((r1 + r2) / 2) + 1;
            return o;
        }

        ~Node() {
            while (child) {
                auto n = child->next;
                delete child;
                child = n;
            }
        }

    };

    Node* roots = nullptr;
    std::size_t count = 0;
public:

    using NodeType = ViolationHeap::Node;

    ViolationHeap() : HeapBase("Violation heap") {};

    bool Empty() const { return count == 0; }

    const Node& Min() const {
        if (!roots)
            EmptyException();

        return *roots;
    }

    const Node* Insert(int key, const Item& item) {
        auto n = new Node(key, item);
        n->next = n;
        ++count;

        AddToRoots(n);

        if (n->key < roots->key)
            roots = n;

        return n;
    }

    void DecreaseKey(const Node* node, int key) {
        if (key > node->key)
            InvalidKeyException();

        auto n = const_cast< Node* >(node);
        n->key = key;
//        throw;

        // if is root stop; if key is smaller make it new min
        if (n->IsRoot()) {
            if (n->key < roots->key)
                roots = n;
            return;
        }

        Node* parent = nullptr;

        // If n is an active node whose new value is not smaller than its parent, stop.
        if ((parent = Node::IsActive(n)) && n->key >= parent->key) {
            return;
        }

        auto lc = n->ActiveChildWithLargetsRank();
        auto curr = n;
        if (lc) {
            n->Replace(lc);
            curr = lc;
        } else {
            if (parent && n == parent->child) {
                parent->child = n->next;
            } else {
                n->prev->next = n->next;
            }

            if (n->next) {
                curr = n->next->prev = n->prev;
            } else {
                curr = parent;
            }
        }

        int oldRank = n->RecalculateRank();


        while (curr && curr->rank < oldRank && (parent = Node::IsActive(curr))) {
            oldRank = parent->RecalculateRank();
            curr = parent;
        }

        n->prev = nullptr;
        n->next = n;

        AddToRoots(n);
        if (n->key < roots->key)
            roots = n;
    }

    std::unique_ptr< Node > ExtractMin() {
        if (!roots)
            EmptyException();

        auto min = roots;
        std::unique_ptr< Node > ret(min);
        PromoteChildren(min);

        --count;
        if (count == 0) {
            roots = nullptr;
            return ret;
        }

        roots = roots->next;
        Consolidate(min);

        return ret;
    }

    ~ViolationHeap() {
        DeleteRoots();
    }

private:

    void AddToRoots(Node* n) {
        if (!roots) {
            roots = n;
            return;
        }

        n->prev = nullptr;
        auto act = n;

        while (act->next && act->next != n) {
            act->prev = nullptr;
            act = act->next;
        }

        act->next = roots->next;
        roots->next = act;
    }

    void AddToRoots(const std::vector< Node* > nodes) {
        for (auto n : nodes) {
            n->prev = nullptr;
            n->next = n;
            AddToRoots(n);
            if (n->key < roots->key)
                roots = n;
        }
    }

    void PromoteChildren(Node* n) {
        auto child = n->child;

        while (child) {
            auto next = child->next;

            child->prev = nullptr;
            child->next = child;
            AddToRoots(child);

            child = next;
        }

        n->child = nullptr;
    }

    void Consolidate(Node* end) {
        int bound = std::ceil(std::log2(count)) + 1;
        int mr = 0;
//        std::vector< std::vector< Node* > > newRoots(bound);
        
        std::vector< Node* > newRoots(bound);
        std::vector< Node* > tmp(count * 2);

        auto act = roots;
        do {
            auto next = act->next;

            auto z = act;
            int r = z->rank;

            while (newRoots[r].size() == 2) {
                auto& v = newRoots[r];
                auto z1 = v[0];
                auto z2 = v[1];

                v.clear();

                if (z->key > z1->key)
                    std::swap(z, z1);

                if (z->key > z2->key)
                    std::swap(z, z2);

                z->AddChild(z1);
                z->AddChild(z2);
                r = ++z->rank;
            }

            newRoots[r].push_back(z);
            act = next;

            if (z->rank > mr)
                mr = z->rank;
        } while (act != end);

        roots = nullptr;

        for (int i = 0; i <= mr; ++i) {
            auto& v = newRoots[i];
            AddToRoots(v);
        }

    }

    void DeleteRoots() {
        if (roots) {
            auto act = roots;
            do {
                auto next = act->next;
                delete act;
                act = next;
            } while (act != roots);
        }
    }

};


}