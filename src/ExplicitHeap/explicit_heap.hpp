#pragma once

#include <memory>
#include <cmath>
#include <functional>
#include "../base/HeapBase.hpp"

namespace MC {

template < typename Item >
class ExplicitHeap : public HeapBase {
    struct Node {
        int key;
        Item item;

        Node* parent = nullptr;

        Node* left = nullptr;
        Node* right = nullptr;

        Node(int k, const Item& i) : key(k), item(i) {}

        ~Node() {
            delete left;
            delete right;
        }

        bool IsRightChild() const {
            if (!parent)
                return false;
            return this == parent->right;
        }

        bool IsLeftChild() const {
            if (!parent)
                return false;
            return this == parent->left;
        }

    };

    Node* root = nullptr;
    Node* last = nullptr;


    void InsertRoot(int key, const Item &item) {
        last = root = new Node(key, item);
    }

    void InsertChild(int key, const Item &item) {
        auto n = new Node(key, item);

        Node* cur = last;
        while (cur != root && cur->IsRightChild()) {
            cur = cur->parent;
        }

        if (cur != root) {
            if (!cur->parent->right) {
                n->parent = cur->parent;
                cur->parent->right = n;
                last = cur->parent->right;
                return;
            }
            cur = cur->parent->right;
        }

        while (cur->left)
            cur = cur->left;

        n->parent = cur;
        cur->left = n;
        last = cur->left;
    }

    Node* Swap(Node* up, Node* lo) {
        std::swap(up->left, lo->left);
        std::swap(up->right, lo->right);

        lo->parent = up->parent;
        up->parent = lo;

        if (up->left)
            up->left->parent = up;
        if (up->right)
            up->right->parent = up;

        if (lo->parent) {
            if (lo->parent->left == up)
                lo->parent->left = lo;
            else
                lo->parent->right = lo;
        } else {
            root = lo;
        }

        if (lo->left == lo)
            lo->left = up;
        else
            lo->right = up;

        if (lo->left)
            lo->left->parent = lo;
        if (lo->right)
            lo->right->parent = lo;

        if (last == lo)
            last = up;

        return lo;
    }

    Node* HeapifyUp(Node* cur) {
        while (cur != root && cur->key < cur->parent->key) {
            cur = Swap(cur->parent, cur);
        }
        return cur;
    }

    std::unique_ptr< Node > DeleteLast() {
        Node* cur = last;

        std::unique_ptr< Node > ret(last);

        if (cur == root)  {
            root = last = nullptr;
            return ret;
        }


        while (cur != root && cur->IsLeftChild()) {
            cur = cur->parent;
        }

        if (cur != root) {
            cur = cur->parent->left;
        }

        while (cur->right) {
            cur = cur->right;
        }

        if (last == last->parent->left)
            last->parent->left = nullptr;
        else
            last->parent->right = nullptr;

        last = cur;
        return ret;
    }

    void HeapifyDown(Node* n) {
        if (!n)
            return;

        Node* smallest = n;

        if (n->left && n->left->key < n->key)
            smallest = n->left;

        if (n->right && n->right->key < smallest->key)
            smallest = n->right;

        if (smallest != n) {
            Swap(n, smallest);
            HeapifyDown(n);
        }
    }



public:
    using NodeType = Node;

    ExplicitHeap() : HeapBase("Explicit heap") {}

    bool Empty() const {
        return root == nullptr;
    }

    ~ExplicitHeap() {
        delete root;
    }

    const Node& Min() const {
        if (!root)
            EmptyException();
        return *root;
    }

    const Node* Insert(int key, const Item& item) {
        if (!root) {
            InsertRoot(key, item);
            return root;
        }

        InsertChild(key, item);
        return HeapifyUp(last);
    }

    void DecreaseKey(const Node* node, int key) {
        auto n = const_cast< Node* >(node);

        if (key > n->key)
            InvalidKeyException();

        n->key = key;
        HeapifyUp(n);
    }

    std::unique_ptr< Node > ExtractMin() {
        if (!root)
            EmptyException();

        auto r = root;
        last->left = r->left;
        last->right = r->right;
        r->left = nullptr;
        r->right = nullptr;

        if (last->left)
            last->left->parent = last;

        if (last->right)
            last->right->parent = last;

        std::swap(last->parent, root->parent);

        if (r->parent) {
            if (r->parent->left == last)
                r->parent->left = r;
            else
                r->parent->right = r;
        }

        root = last;
        last = r;

        auto ret = DeleteLast();

        HeapifyDown(root);

        return ret;
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