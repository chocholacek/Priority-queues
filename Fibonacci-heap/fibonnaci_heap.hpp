#pragma once

#include <cmath>
#include <list>
#include <memory>
#include <vector>


namespace PriorityQueues {
//template < typename T >
class fibonacci_heap {
public:
    struct node {
        int value = 0;
        unsigned degree = 0;
        bool mark = false;

        node* next;
        node* prev;

        node* child = nullptr;
        node* parent = nullptr;

        node(int value) : value(value) {
            reset();
        }

        void reset() {
            next = prev = this;
        }

        void add_child(node* n) {
            n->parent = this;
            ++degree;
            if (child == nullptr) {
                n->next = n;
                n->prev = n;
                child = n;
                return;
            }

            child->add_sibling(n);
        }

        void add_sibling(node* n) {
            n->prev = this;
            next->prev = n;
            n->next = next;
            next = n;
        }

        node* remove() {
            next->prev = prev;
            prev->next = next;
            return this;
        }
    };


    node* _min = nullptr;
    unsigned _count = 0;
    unsigned _root_size = 0;

    void insert(int v) {
        node* n = new node(v);
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

    node* extract_min() {
        node* ret = _min;
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
        return ret;
    }

    void _consolidate() {
        unsigned bound = std::log2(_count) + 1;
        std::vector<node*> array(bound, nullptr);
        //node* array[bound] = {nullptr};
        node* actual = _min;
        for (;_root_size > 0; --_root_size) {
            node* x = actual;
            unsigned d = x->degree;

            while (array[d] != nullptr) {
                node* y = array[d];
                if (x->value > y->value)
                    std::swap(x, y);
                _fib_heap_link(y, x);
                array[d] = nullptr;
                ++d;
            }

            array[d] = x;
            actual = actual->next;
        }

        _min = nullptr;
        for (int i = 0; i < bound; ++i) {
            if (array[i] != nullptr) {
                ++_root_size;
                if (_min == nullptr) {
                    _min = array[i];
                } else {
                    _min->add_sibling(array[i]);
                    if (array[i]->value < _min->value)
                        _min = array[i];
                }
            }
        }
    }

    void _fib_heap_link(node* y, node* x) {
        y->remove();
        x->add_child(y);
        y->mark = false;
    }

    void _move_children_to_root(node* n) {
        node* p = n->child;
        n->child = nullptr;
        if (p != nullptr) {
            do {
                p->parent = nullptr;
                _min->add_sibling(p);
                p = p->next;
            } while (p != n->child);
        }
    }

//protected:
//    struct node;
//    using unique_ptr = std::unique_ptr< node >;
//    using shared_ptr = std::shared_ptr< node >;
//    using weak_ptr = std::weak_ptr< node >;
//
//    static constexpr double _log_g = 0.4812118;
//
//    std::size_t _n = 0;
//    shared_ptr _min;
//    std::size_t _root_size = 0;
//
//public:
//    template < typename... Args >
//    void insert(Args&&... args) {
//        static_assert(std::is_constructible_v< T, Args... >, "Provided type can't be constructed with provided arguments");
//        if (!_min) {
//            _min = node::make(std::forward< Args >(args)...);
//        } else {
//            _min->add_right(node::make(std::forward< Args >(args)...));
//            if (_min->right->key < _min->key) {
//                _min = _min->right;
//            }
//        }
//        ++_n;
//    }
//
//    T remove_min() {
//        T ret = _min->key;
//        --_root_size;
//        _add_children_to_root(_min);
//        return ret;
//    }
//protected:
//
//    struct node : std::enable_shared_from_this<node> {
//        T key;
//        unsigned degree = 0;
//        bool mark = false;
//        node* parent = nullptr;
//        shared_ptr children;
//        shared_ptr right;
//        weak_ptr left;
//
//        template < typename... Args >
//        explicit node(Args&&... args) : key(std::forward< Args >(args)...) {}
//
//        template < typename... Args >
//        static shared_ptr make(Args&&... args) {
//            auto p = std::make_shared< node >(std::forward< Args >(args)...);
//            p->initialize_self();
//            return p;
//        }
//
//
//        shared_ptr initialize_self() {
//            right = this->shared_from_this();
//            left = this->weak_from_this();
//            return right;
//        }
//
//        shared_ptr remove_self() {
//            shared_ptr self = this->shared_from_this();
//            right->left = left;
//            left.lock()->right = std::move(right);
//            return self;
//        }
//
//        node& add_right(shared_ptr&& r) {
//            auto tmp = std::move(right);
//            right = std::move(r);
//            right->left = tmp->left;
//            tmp->left = right;
//            right->right = std::move(tmp);
//            return *this;
//        }
//
//        node& add_child(shared_ptr&& c) {
//            c->mark = false;
//            if (!children) {
//                c->parent = this;
//                children = std::move(c);
//            } else {
//                children->add_right(std::move(c));
//            }
//            ++degree;
//            return *this;
//        }
//    }; // node
//
//    void _consolidate() {
//        unsigned bound = static_cast<unsigned>(std::log(_n) / _log_g) + 1;
//        shared_ptr array[bound];
//        unsigned d = 0;
//
//        for (auto p = _min; _root_size > 0; --_root_size) {
//            shared_ptr parent = p;
//            d = parent->degree;
//            p = p->right;
//
//            while (array[d] != nullptr) {
//                shared_ptr child = array[d];
//                if (child->key < parent->key) {
//                    std::swap(child, parent);
//                }
//                parent->add_child(child->remove_self());
//                array[d++].reset();
//            }
//            array[d] = std::move(parent);
//        }
//
//        d = 0;
//        while (!array[d++]);
//
//        _min = array[d - 1]->initialize_self();
//        for (; d < bound; ++d) {
//            if (array[d]) {
//                array[d]->degree = d;
//                _min->add_right(std::move(array[d]));
//                if (_min->right->key < _min->key) {
//                    _min = _min->right;
//                }
//                ++_root_size;
//            }
//        }
//        ++_root_size;
//    }
//
//    void _add_children_to_root(shared_ptr n) {
//        shared_ptr child = std::move(n->children);
//        for (int i = 0; i < n->degree; ++i) {
//            _add_children_to_root(std::move(child));
//            child = std::move(child->right);
//            ++_root_size;
//        }
//    }
};
}