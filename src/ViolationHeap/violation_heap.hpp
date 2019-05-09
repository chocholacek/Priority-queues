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
            rank = std::ceil((r1 + r2) / 2.0) + 1;
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

    struct ZS {
        std::array< Node*, 2 > data;
        int count = 0;

        void Add(Node* n) { data[count++] = n; }

        Node* Last() noexcept { return data[count - 1]; }

        Node* z1() const noexcept { return data[0]; }
        Node* z2() const noexcept { return data[1]; }

        ZS() : data({nullptr, nullptr}) {}

        void Reset() {
            count = 0;
            data[1] = data[0] = nullptr;
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
        if (lc) {
            n->Replace(lc);
        } else {
            if (parent && n == parent->child) {
                parent->child = n->next;
            } else {
                n->prev->next = n->next;
            }

            if (n->next) {
                n->next->prev = n->prev;
            }
        }

        n->RecalculateRank();
        auto curr = parent;
        if (curr) {
            int oldRank = curr->RecalculateRank();

            while (curr->rank < oldRank && (parent = Node::IsActive(curr))) {
                oldRank = parent->RecalculateRank();
                curr = parent;
            }
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

    void AddToRoots(ZS& zs) {
        for (;zs.count; --zs.count) {
            auto n = zs.Last();
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
        int mr = 0;
        unsigned bound = std::ceil(std::log2(count) + 1);
        std::vector< ZS > tmp(bound);

        auto z = roots;
        do {
            auto next = z->next;

            ZS zs = tmp[z->rank];
            while (zs.count == 2) {
                auto z1 = zs.z1();
                auto z2 = zs.z2();

                if (z->key > z1->key)
                    std::swap(z, z1);

                if (z->key > z2->key)
                    std::swap(z, z2);

                if (z1->rank > z2->rank)
                    std::swap(z1, z2);

                z->AddChild(z1);
                z->AddChild(z2);

                tmp[z->rank].Reset();
                ++z->rank;
                zs = tmp[z->rank];
            }

            tmp[z->rank].Add(z);

            if (z->rank > mr)
                mr = z->rank;
            z = next;
        } while (z != end);

        roots = nullptr;

        for (int i = 0; i <= mr; ++i) {
            AddToRoots(tmp[i]);
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