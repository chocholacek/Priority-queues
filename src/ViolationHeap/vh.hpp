#pragma once

#include <functional>

namespace MC {


template<typename Item>
class ViolationHeap {
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
        Node* GetLast(std::function< void(Node*) > action = [](Node*){}) {
            auto curr = this;
            while (curr->next != this) {
                action(curr);
                curr = curr->next;
            }
            return curr;
        }

        void Unlink() {
            next = prev = nullptr;
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
                child = child->next;
                delete child;
            }
        }
    };

    Node* roots = nullptr;
    std::size_t size = 0;
public:

    const Node& Min() const {
        if (!roots)
            throw std::logic_error("empty heap");

        return *roots;
    }

    const Node& Insert(int key, const Item& item) {
        auto n = new Node(key, item);

        Meld(n);
        ++size;

        return *n;
    }

    void DecreaseKey(const Node& node, int key) {
        if (key > node.key)
            throw std::logic_error("larger key provided");

        auto n = const_cast< Node* >(&node);
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
            throw std::logic_error("empty heap");

        auto min = roots;
        auto children = min->child;
        min->child = nullptr;
        Item i = min->item;

        min->Free();
        delete min;

        roots = roots->next;
        Meld(children);
        --size;
        Consolidate();

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
            delete n;
            n = m;
        } while (n != roots);
    }

    void Consolidate() {
        if (!roots)
            return;


        // TODO - kokotina

        std::vector< Node* > rl(2 * size, nullptr);

        int i = 0;
        for (auto p = roots; p; p = p->next) {
            auto z = rl[i]
        }

    }
};


}