#pragma once

#include <cmath>
#include <list>
#include <memory>
#include <vector>
#include <optional>

#include "../base/HeapBase.hpp"

namespace MC {
template < typename Item >
class FibonacciHeap : HeapBase {
    struct Node {
        int key;
        Item item;
        unsigned degree = 0;
        bool mark = false;

        Node* next;
        Node* prev;

        Node* child = nullptr;
        Node* parent = nullptr;

        template < typename I >
        Node(int k, I&& i)
                : key(k),
                  item(std::forward< I >(i)) {
            Reset();
        }

        void Reset() {
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
    };

    Node* _min = nullptr;
    unsigned _count = 0;
    unsigned _root_size = 0;

public:

    using keyitem = std::pair< int, const Item& >;
    using optional = std::optional< keyitem >;

    const Node& Min() const {
        if (!_min)
            EmptyException();

        return *_min;
    }

    void Insert(int key, const Item& item) {
        Node* n = new Node(key, item);
        ++_count;
        ++_root_size;

        if (_min == nullptr) {
            _min = n;
            return;
        }

        _min->AddSibling(n);
        if (n->key < _min->key) {
            _min = n;
        }
    }

    Item ExtractMin() {
        if (!_min)
             EmptyException();


        Node* ret = _min;
        _move_children_to_root(_min);
        _min->Remove();
        --_root_size;
        if (ret == ret->next) {
            _min = nullptr;
        } else {
            _min = ret->next;
            _consolidate();
        }
        --_count;

        Item i = ret->item;
        ret->Reset();
        _clear(ret);
        return i;

    }

    void DecreaseKey(const Node& node, int k) {
        auto x = const_cast< Node* >(&node);

        if (k > x->key)
            InvalidKeyException();

        x->key = k;
        Node* y = x->parent;
        if (y != nullptr && x->key > y->key) {
            _cut(x, y);
            _cascading_cut(y);
        }
        if (x->key < _min->key)
            _min = x;
    }

    ~FibonacciHeap() {
        _clear(_min);
    }


private:

    void _consolidate() {
        unsigned bound = std::log2(_count) + 1;
        std::vector< Node* > array(bound, nullptr);
        Node* actual = _min;
        for (;_root_size > 0; --_root_size) {
            Node* x = actual;
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
            actual = actual->next;
        }

        _min = nullptr;
        for (unsigned i = 0; i < bound; ++i) {
            if (array[i] != nullptr) {
                ++_root_size;
                if (_min == nullptr) {
                    _min = array[i];
                } else {
                    _min->AddSibling(array[i]);
                    if (array[i]->key < _min->key)
                        _min = array[i];
                }
            }
        }
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

    void _clear(Node *n) {
        if (n) {
            auto act = n;
            do {
                auto d = act;
                act = act->next;
                _clear(d->child);
                delete d;
            } while (act != n);
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

        _min->AddSibling(x);
        x->parent = nullptr;
        x->mark = false;
    }

    void _cascading_cut(Node* y) {
        Node* z = y->parent;
        if (z != nullptr) {
            if (!y->mark) {
                y->mark = true;
            } else {
                _cut(y, z);
                _cascading_cut(z);
            }
        }
    }
};
}