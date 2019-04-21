#pragma once

#include "../base/HeapBase.hpp"

namespace MC {

template < typename Item >
class RankPairingHeap : HeapBase {
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
    };

    Node* root = nullptr;
    std::size_t size = 0;

public:

    using ItemType = Item;

    const Node& Min() const {
        if (!root)
            EmptyException();
        return *root;
    }

    const Node& Insert(int key,const Item& item) {
        auto n = new Node(key, item);
        AddToRootList(n);
        ++size;

        return *n;
    }

    Item ExtractMin() {
        if (!root)
            EmptyException();

        std::vector< Node* > buckets(MaxBuckets(), nullptr);

        // traverse children
        for (auto n = root ->left; n != nullptr;) {
            auto next = n->ResetNextAndParent();
            MultiPass(buckets, n);
            n = next;
        }

        // traverse roots
        for (auto n = root->next; n != root;) {
            auto next = n->ResetNext();
            MultiPass(buckets, n);
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

    void DecreaseKey(const Node& node, int key) {
        if (key > node.key)
            InvalidKeyException();

        auto n = const_cast< Node* >(&node);
        n->key = key;

        if (!n->parent) {
            if (n->key < root->key)
                root = n;
        }
    }

private:

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

    void MultiPass(std::vector< Node* >& buckets, Node* n) {
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