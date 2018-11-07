#pragma once

#include <cmath>
#include <list>
#include <memory>
#include <vector>


namespace PriorityQueues {
//template < typename T >
class fibonacci_heap {
protected:
    struct node;

    using shared_ptr = std::shared_ptr< node >;
    using weak_ptr = std::weak_ptr< node >;


private:
    shared_ptr _min = nullptr;
    unsigned _count = 0;
    unsigned _root_size = 0;

public:
    void insert(int v) {
        auto n = node::make(v);
        ++_count;
        ++_root_size;

        if (_min == nullptr) {
            _min = n;
            return;
        }

        _min->add_sibling(n);
        if (n->value < _min->value) {
            _min = n;
        }
    }

    int extract_min() {
        if (_count == 0)
            throw;
        shared_ptr ret = _min;
        if (ret != nullptr) {
            _move_children_to_root(_min);
            _min->remove();
            --_root_size;
            if (ret == ret->next) {
                _min = nullptr;
            } else {
                _min = ret->next;
                _consolidate();
            }
            --_count;
        }
        return ret->value;
    }

protected:
    struct node : std::enable_shared_from_this< node > {
        int value = 0;
        unsigned degree = 0;
        bool mark = false;

        shared_ptr next;
        weak_ptr prev;

        shared_ptr child;
        node* parent = nullptr;

        node(int value) : value(value) {}
        
        static shared_ptr make(int v) {
            auto p = std::make_shared< node >(v);
            p->reset();
            return p;
        }

        void reset() {
            next = this->shared_from_this();
            prev = this->weak_from_this();
        }

        void add_child(shared_ptr n) {
            n->parent = this;
            ++degree;
            if (child == nullptr) {
                n->reset();
                child = n;
                return;
            }

            child->add_sibling(std::move(n));
        }

        void add_sibling(shared_ptr n) {
            n->prev = this->weak_from_this();
            next->prev = n;
            n->next = next;
            next = std::move(n);
        }

        node* remove() {
            next->prev = prev;
            prev.lock()->next = next;
            return this;
        }
    };

    void _consolidate() {
        unsigned bound = std::log2(_count) + 1;
        std::vector< shared_ptr > array(bound);
        shared_ptr actual = _min;
        for (;_root_size > 0; --_root_size) {
            shared_ptr x = actual;
            unsigned d = x->degree;

            while (array[d]) {
                shared_ptr y = array[d];
                if (x->value > y->value)
                    std::swap(x, y);
                _fib_heap_link(y, x);
                array[d].reset();
                ++d;
            }

            array[d] = x;
            actual = actual->next;
        }

        _min = nullptr;
        for (unsigned i = 0; i < bound; ++i) {
            if (array[i]) {
                ++_root_size;
                if (!_min) {
                    _min = std::move(array[i]);
                } else {
                    _min->add_sibling(array[i]);
                    if (array[i]->value < _min->value)
                        _min = array[i];
                }
            }
        }
    }

    void _fib_heap_link(shared_ptr y, shared_ptr x) {
        y->remove();
        x->add_child(y);
        y->mark = false;
    }

    void _move_children_to_root(shared_ptr n) {
        shared_ptr p = n->child;
        n->child.reset();
        if (p) {
            do {
                p->parent = nullptr;
                _min->add_sibling(p);
                p = p->next;
            } while (p != n->child);
        }
    }
};
}