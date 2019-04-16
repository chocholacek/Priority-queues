#pragma once

#include <memory>
#include <cmath>
#include <functional>
#include "../../base/HeapBase.hpp"

namespace MC {

template < typename Item >
class ExplicitHeap : public HeapBase {
    struct Node {
        int key;
        Item item;

        Node* parent;

        std::unique_ptr< Node > left;
        std::unique_ptr< Node > right;

        Node(int k, const Item& i, Node* p = nullptr) : key(k), item(i), parent(p) {}

        void Swap(Node& n) {
            Node tmp(key, item);
            key = n.key;
            item = n.item;
            n.key = tmp.key;
            n.item = tmp.item;
        }
    };

    std::unique_ptr< Node > root;
    Node* last = nullptr;


    void InsertRoot(int key, const Item &item) {
        root = std::make_unique< Node >(key, item);
        last = root.get();
    }

    void InsertChild(int key, const Item &item) {
        Node* cur = last;
        while (cur != root.get() && IsRightChild(cur, cur->parent)) {
            cur = cur->parent;
        }

        if (cur != root.get()) {
            if (!cur->parent->right) {
                cur->parent->right = std::make_unique< Node >(key, item, cur->parent);
                last = cur->parent->right.get();
                return;
            }
            cur = cur->parent->right.get();
        }

        while (cur->left)
            cur = cur->left.get();

        cur->left = std::make_unique< Node >(key, item, cur);
        last = cur->left.get();
    }

    bool IsRightChild(Node* child, Node* of) const {
        return child == of->right.get();
    }

    bool IsLeftChild(Node* child, Node* of) const {
        return !IsRightChild(child, of);
    }

    void HeapifyUp(Node* cur) {
        while (cur != root.get() && cur->key < cur->parent->key) {
            cur->Swap(*cur->parent);
            cur = cur->parent;
        }
    }

    void DeleteLast() {
        Node* cur = last;

        if (cur == root.get()) {
            root.reset();
            last = nullptr;
            return;
        }


        while (cur != root.get() && IsLeftChild(cur, cur->parent)) {
            cur = cur->parent;
        }

        if (cur != root.get()) {
            cur = cur->parent->left.get();
        }

        while (cur->right) {
            cur = cur->right.get();
        }

        if (last == last->parent->left.get())
            last->parent->left.reset();
        else
            last->parent->right.reset();

        last = cur;
    }

    void HeapifyDown(Node* n) {
        if (!n)
            return;

        Node* smallest = n;

        if (n->left && n->left->key < n->key)
            smallest = n->left.get();

        if (n->right && n->right->key < smallest->key)
            smallest = n->right.get();

        if (smallest != n) {
            n->Swap(*smallest);
            HeapifyDown(smallest);
        }
    }


public:

    using NodeType = Node;

    const Node& Min() const { return *root; }

    void Insert(int key, const Item& item) {
        if (!root) {
            InsertRoot(key, item);
            return;
        }

        InsertChild(key, item);
        HeapifyUp(last);
    }

    void DecreaseKey(const Node& node, int key) {
        auto n = const_cast< Node &>(node);

        if (key > n.key)
            throw;

        n.key = key;
        HeapifyUp(&n);
    }

    Item ExtractMin() {
        auto i = Min().item;

        root->Swap(*last);

        DeleteLast();

        HeapifyDown(root.get());
        return i;
    }
};


template < typename T >
std::vector< T > preorder(const ExplicitHeap< T >& h) {
    std::vector< T > v;

    std::function< void(const typename ExplicitHeap< T >::NodeType&) > pre;
    pre = [&v, &pre](const auto& n) -> void {
        v.push_back(n.key);
        if (n.left)
            pre(*n.left);
        if (n.right)
            pre(*n.right);
    };

    pre(h.Min());
    return v;
}

}