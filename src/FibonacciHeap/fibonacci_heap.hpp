#pragma once

#include <cmath>
#include <list>
#include <memory>
#include <vector>
#include <optional>

#include "../base/HeapBase.hpp"

namespace MC {

template < typename Item >
class BinomialHeap;

template < typename Item >
class FibonacciHeap : public HeapBase {
    struct Node {
        int key;
        Item item;
        unsigned degree = 0;
        bool mark = false;

        Node* next = nullptr;
        Node* prev = nullptr;

        Node* child = nullptr;
        Node* parent = nullptr;

        Node(int k, const Item& i)
                : key(k), item(i) {}

        void ResetAll() {
            ResetSiblings();
            child = parent = nullptr;
        }

        void ResetSiblings() {
            next = prev = this;
        }

        void AddChild(Node* n) {
            n->parent = this;
            ++degree;
            if (child == nullptr) {
                n->next = n;
                n->prev = n;
                child = n;
                return;
            }

            child->AddSibling(n);
        }

        void AddSibling(Node* n) {
            n->prev = this;
            next->prev = n;
            n->next = next;
            next = n;
        }

        Node* Remove() {
            next->prev = prev;
            prev->next = next;
            return this;
        }

        bool IsAlone() const {
            return this == next;
        }

        ~Node() {
            if (child) {
                auto act = child;
                do {
                    auto next = act->next;
                    delete act;
                    act = next;
                } while (act != child);
            }
        }
    };

    Node* _min = nullptr;
    unsigned _count = 0;
    unsigned _root_size = 0;

public:

    using NodeType = FibonacciHeap::Node;

    FibonacciHeap() : HeapBase("Fibonacci heap") {}

    bool Empty() const {
        return _count == 0;
    }

    const Node& Min() const {
        if (!_min)
            EmptyException();

        return *_min;
    }

    const Node* Insert(int key, const Item& item) {
        auto n = new Node(key, item);
        ++_count;

        _add_to_root(n);

        return n;
    }

    std::unique_ptr< Node > ExtractMin() {
        if (!_min)
             EmptyException();

        _move_children_to_root(_min);
        _min->Remove();
        std::unique_ptr< Node > ret(_min);

        --_root_size;
        if (_root_size == 0) {
            _min = nullptr;
        } else {
            _min = ret->next;
            _consolidate();
        }
        --_count;
        ret->ResetAll();
        return ret;

    }

    void DecreaseKey(const Node* node, int k) {
        auto x = const_cast< Node* >(node);

        if (k > x->key)
            InvalidKeyException();

        x->key = k;
        Node* y = x->parent;
        if (y && x->key < y->key) {
            _cut(x, y);
            _cascading_cut(y);
        }
        if (x->key < _min->key)
            _min = x;
    }

    ~FibonacciHeap() {
        if (!_min)
            return;
        _clear();
    }


protected:

    template <typename > friend class BinomialHeap;
    FibonacciHeap(const std::string& derived) : HeapBase(derived) {}

    void _consolidate() {
        unsigned bound = std::ceil(std::log2(_count) + 1);
        std::vector< Node* > array(bound, nullptr);
        Node* actual = _min;
        for (;_root_size > 0; --_root_size) {
            auto x = actual;
            auto n = x->next;
            unsigned d = x->degree;

            while (array[d] != nullptr) {
                Node* y = array[d];
                if (x->key > y->key)
                    std::swap(x, y);
                _fib_heap_link(y, x);
                array[d] = nullptr;
                ++d;
            }

            array[d] = x;
            actual = n;
        }

        _min = nullptr;
        for (auto r : array)
            _add_to_root(r);
    }

    void _fib_heap_link(Node* y, Node* x) {
        y->Remove();
        x->AddChild(y);
        y->mark = false;
    }

    void _move_children_to_root(Node* n) {
        Node* p = n->child;
        Node* next = nullptr;
        if (p != nullptr) {
            for (unsigned i = 0; i < n->degree; ++i, p = next) {
                p->parent = nullptr;
                next = p->next;
                _min->AddSibling(p);
                ++_root_size;
            }
            n->child = nullptr;
        }
    }



    void _cut(Node* x, Node* y) {
        // Remove child
        if (y->child == x) {
            y->child = x->next;
        }
        x->Remove();
        --y->degree;
        if (y->degree == 0) {
            y ->child = nullptr;
        }

        _add_to_root(x);
        x->parent = nullptr;
        x->mark = false;
    }

    void _cascading_cut(Node* y) {
        Node* z = y->parent;
        if (z) {
            if (!y->mark) {
                y->mark = true;
            } else {
                _cut(y, z);
                _cascading_cut(z);
            }
        }
    }

    void _clear() {
        auto act = _min;
        for (; _root_size; --_root_size) {
            auto n = act->next;
            delete act;
            act = n;
        }
        _count = 0;
    }

    void _add_to_root(Node* n) {
        if (!n)
            return;
        ++_root_size;
        n->ResetSiblings();
        if (!_min) {
            _min = n;
        } else {
            _min->AddSibling(n);
            if (n->key < _min->key)
                _min = n;
        }
    }
};
}