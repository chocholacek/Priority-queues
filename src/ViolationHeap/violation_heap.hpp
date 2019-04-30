#pragma once

#include <cmath>
#include <functional>
#include "../base/HeapBase.hpp"

namespace MC {

static int bin = 0;

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

        Node(int k, const Item& i) : key(k), item(i) {
            AssignSelf();
        }



    private:
        Node* GetLast(std::function< void(Node*) > action = [](Node*){}, int& count = bin) {
            auto curr = this;
            while (curr->next != this) {
                action(curr);
                ++count;
                curr = curr->next;
            }
            return curr;
        }

        void Unlink() {
            next = prev = nullptr;
        }

        void Link(Node* n) {
            auto c = child;

            child = n;
            n->prev = this;

            n->next = c;
            if (c)
                c->prev = n;
        }

        void AssignSelf() {
            next = this;
        }

        bool IsRoot() const {
            return prev == nullptr;
        }

        // returns parent if this node is active
        // nullptr otherwise
        Node* Active() const {
            auto curr = prev;

            if (this == curr->child)
                return curr;

            curr = curr->prev;

            if (this == curr->child)
                return curr;

            return nullptr;
        }

        // recalculates rank and returns old rank
        int RecalculateRank() {
            if (!child)
                return rank;

            auto c2 = child->next;

            float rc1 = child->rank;
            float rc2 = c2 ? c2->rank : -1;

            int r = rank;
            rank = static_cast< int >(std::ceil((rc1 + rc2) / 2) + 1);
            return r;
        }

        Node* LargestActiveChild() const {
            if (!child)
                return nullptr;

            int rc1 = child->rank;

            if (!child->next)
                return child;

            int rc2 = child->next->rank;

            return rc2 > rc1 ? child->next : child;
        }

        void Free() {
            while (child) {
                child->Free();
                auto n = child->next;
                delete child;
                child = n;
            }
        }
    };

    Node* roots = nullptr;
public:

    using NodeType = ViolationHeap::Node;

    ViolationHeap() : HeapBase("Violation heap") {};

    const Node& Min() const {
        if (!roots)
            EmptyException();

        return *roots;
    }

    const Node* Insert(int key, const Item& item) {
        auto n = new Node(key, item);
        Meld(n);
        return n;
    }

    void DecreaseKey(const Node* node, int key) {
        if (key > node->key)
            InvalidKeyException();

        auto n = const_cast< Node* >(node);
        n->key = key;

        if (n->IsRoot()) {
            if (n->key < roots->key)
                roots = n;
            return;
        }

        Node* p = nullptr;

        if ((p = n->Active()) && p->key <= n->key)
            return;


        int oldRank = n->RecalculateRank();
        auto curr = n;
        while (!curr->IsRoot() && oldRank > curr->rank && (p = curr->Active())) {
            oldRank = p->RecalculateRank();
            curr = p;
        }

        auto lc = n->LargestActiveChild();

        Cut(lc);
        lc->next = n->next;
        lc->prev = n->prev;

        // n is first child
        if (n == n->prev->child) {
            n->prev->child = lc;

        } else {
            n->prev->next = lc;
        }

        if (n->next)
            n->next->prev = lc;

        n->AssignSelf();
        Meld(n);

    }

    Item ExtractMin() {
        if (!roots)
            EmptyException();

        auto min = roots;
        auto children = min->child;
        min->child = nullptr;
        Item i = min->item;

        roots = roots->next;
        delete min;

        if (roots != min) {
            Meld(children);
            Consolidate(min);
        } else {
            roots = nullptr;
        }

        return i;
    }

    ~ViolationHeap() {
        DeleteRoots();
    }

private:

    void Cut(Node* n) {
        // n is first child
        if (n == n->prev->child) {
            n->prev->child = n->next;

        } else {
            n->prev->next = n->next;
        }

        if (n->next)
            n->next->prev = n->prev;

    }

    void Meld(Node* h) {
        if (!h)
            return;

        if (!roots) {
            roots = h;
            return;
        }

        if (h->key < roots->key)
            std::swap(roots,h);

        auto next = roots->next;
        roots->next = h;
        auto curr = h->GetLast([](auto* n) { n->prev = nullptr; });

        curr->next = next;
    }

    void DeleteRoots() {
        if (!roots)
            return;

        auto n = roots;
        do {
            auto m = n->next;
            n->Free();
            delete n;
            n = m;
        } while (n != roots);
    }

    void Consolidate(Node* end) {
        if (!roots)
            return;

        std::vector< std::vector< Node* >> ranks(64);

        auto z = roots;
        do {
            auto r = z->rank;

            while (ranks[r].size() == 2) {
                auto& v = ranks[r];
                auto z1 = v.back();
                v.pop_back();
                auto z2 = v.back();
                v.pop_back();

                if (z1->key < z->key)
                    std::swap(z1, z);

                if (z2->key < z->key)
                    std::swap(z2, z);

                z->Link(z1);
                z->Link(z2);

                r = ++z->rank;
            }

            ranks[r].push_back(z);
            auto next = z->next;

            z->Unlink();

            z = next;
        } while (z != end);

        roots = nullptr;
        for (auto& v : ranks) {
            while (v.size()) {
                auto n = v.back();
                n->AssignSelf();
                Meld(n);
                v.pop_back();
            }
        }

    }
};


}