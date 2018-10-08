#pragma once

#include <memory>
#include <list>

namespace PriorityQueues {

template < typename T, typename Compare = std::less< T > >
struct circular_list {

    struct node;

    using weak_ptr = std::weak_ptr< node >;
    using shared_ptr = std::shared_ptr< node >;

    struct node {
        T value;
        std::size_t degree = 0;
        weak_ptr left;
        shared_ptr right;
        shared_ptr child;
        weak_ptr parent;
        circular_list children;

        template < typename... Args >
        node(Args&&... args)
                : value(std::forward< Args >(args)...) {}

        bool operator==(const node& n) {
            return this == &n;
        }
    };

    shared_ptr root;

public:
    template < typename... Args >
    void insert(Args&&... args) {
        if (!root) {
            root = make_shared(std::forward< Args >(args)...);
            root->right = root;
            root->left = root;
            return;
        }
        auto p = make_shared(std::forward< Args >(args)...);
        p->right = std::move(root->right);
        p->right->left = p;
        root->right = p;
        p->left = root;
        if (Compare()(p->value, root->value))
            root = p;
    }

    void insert(circular_list&& l) {
        if (!l.root)
            return;

        if (!root) {
            root = std::move(l.root);
            return;
        }

        l.root->left.lock()->right = std::move(root->right);
        l.root->left.lock()->right->left = l.root->left;
        l.root->left = root;
        root->right = l.root;
        if (Compare()(l.root->value, root->value))
            root = std::move(l.root);
        else
            l.root.reset();
    }

    ~circular_list() {
        if (root)
            root->right.reset();
    }
private:
    template < typename... Args >
    shared_ptr make_shared(Args&&... args) {
        return std::make_shared< node >(std::forward< Args >(args)...);
    }
};

}