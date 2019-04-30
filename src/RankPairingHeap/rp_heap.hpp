#pragma once

#include "../base/HeapBase.hpp"

namespace MC {

template < typename Item >
class RankPairingHeap : public HeapBase {
protected:
    struct Node {
        int key;
        Item item;

        Node* left = nullptr;
        Node* next = nullptr;
        Node* parent = nullptr;

        int rank = 0;

        Node(int k, const Item& i)
            : key(k), item(i) {}

        Node* ResetNext() {
            auto n = next;
            next = nullptr;
            return n;
        }

        Node* ResetNextAndParent() {
            parent = nullptr;
            return ResetNext();
        }

        bool IsRoot() const {
            return parent == nullptr;
        }

        void RecalculateRank() {
            rank = left ? left->rank + 1 : 0;
        }

        void Free() {
            if (left) {
                left->Free();
                delete left;
            }

            if (!IsRoot() && next) {
                next->Free();
                delete next;
            }

        }
    };

    Node* root = nullptr;
    std::size_t size = 0;

public:
    using NodeType = RankPairingHeap::Node;
    using ItemType = Item;

    RankPairingHeap() : HeapBase("Rank-pairing heap") {}

    const Node& Min() const {
        if (!root)
            EmptyException();
        return *root;
    }

    const Node* Insert(int key,const Item& item) {
        auto n = new Node(key, item);
        AddToRootList(n);
        ++size;

        return n;
    }

    Item ExtractMin() {
        if (!root)
            EmptyException();

        std::vector< Node* > buckets(MaxBuckets(), nullptr);

        // traverse children
        for (auto n = root->left; n != nullptr;) {
            auto next = n->ResetNextAndParent();
            MergeIntoBuckets(buckets, n);
            n = next;
        }

        // traverse roots
        for (auto n = root->next; n != root;) {
            auto next = n->ResetNext();
            MergeIntoBuckets(buckets, n);
            n = next;
        }

        Item i = root->item;
        Free(root);
        root = nullptr;
        std::for_each(buckets.begin(), buckets.end(), [this](auto n) {
            AddToRootList(n);
        });

        return i;
    }

    void DecreaseKey(const Node* node, int key) {
        if (key > node->key)
            InvalidKeyException();

        auto n = const_cast< Node* >(node);
        n->key = key;

        if (n->IsRoot()) {
            if (n->key < root->key)
                root = n;
            return;
        }

        auto p = n->parent;

        if (n == p->left) {
            p->left = n->next;
            if (p->left)
                p->left->parent = p;
        } else {
            p->next = n->next;
            if (p->next)
                p->next->parent = p;
        }

        n->ResetNextAndParent();
        n->RecalculateRank();

        AddToRootList(n);

        if (p->IsRoot()) {
            p->RecalculateRank();
        } else {
            ReduceRanks(p);
        }
    }

    ~RankPairingHeap() {
        FreeRoots();
    }

private:

    void FreeRoots() {
        if (!root)
            return;

        auto n = root;
        do {
            auto p = n->next;
            n->Free();
            delete n;
            n = p;
        } while (n != root);

        root = nullptr;
    }

    // type-1 rank reduction
    void ReduceRanks(Node* n) {
        while (!n->IsRoot()) {
            int x = n->left ? n->left->rank : -1;
            int y = n->next ? n->next->rank : -1;

            int z = x != y ? std::max(x, y) : x + 1;

            if (z >= n->rank)
                break;

            n->rank = z;
            n = n->parent;

        }
    }

    void AddToRootList(Node* n) {
        if (!n)
            return;

        if (!root) {
            root = n;
            n->next = n;
        } else {
            n->next = root->next;
            root->next = n;
            if (n->key < root->key)
                root = n;
        }
    }

    std::size_t MaxBuckets() const {
        return std::ceil(std::log2(size)) + 1;
    }

    Node* Link(Node* x, Node* y) {
        if (!y)
            return x;

        if (y->key < x->key)
            std::swap(x, y);

        y->parent = x;
        if (x->left) {
            y->next = x->left;
            y->next->parent = y;
        }

        x->left = y;
        x->rank = y->rank + 1;
        return x;
    }

    void MergeIntoBuckets(std::vector< Node* >& buckets, Node* n) {
        while (buckets[n->rank]) {
            auto b = buckets[n->rank];
            buckets[n->rank] = nullptr;
            n = Link(n, b);
        }

        buckets[n->rank] = n;
    }

    void Free(Node* n) {
        delete n;
        --size;
    }
};

}