#pragma once

#include <memory>

namespace MC {


template < typename Item >
class ViolationHeap {
protected:
    struct Node: std::enable_shared_from_this< Node > {
        friend ViolationHeap;

        int key;
        Item item;

        int rank = 0;

        std::shared_ptr< Node > next;
        std::weak_ptr< Node > prev;
        std::unique_ptr< Node > child;

        Node(int k, const Item& i) : key(k), item(i) {}

        // root node is in single-linked list
        // thus prev pointer is not used (set to nullptr)
        bool IsRoot() const {
            return !prev.use_count();
        }

    private:

        const Node& GetLast() const {
            return GetLastImpl< const Node >(*this);
        }

        Node& GetLast() {
            return GetLastImpl< Node >(*this);
        }

        void SetSelfSibling() {
            next = this->shared_from_this();
        }

        template < typename IN >
        static IN& GetLastImpl(IN& n) {
            auto* curr = &n;
            while (curr->next.get() != &n) {
                curr = curr->next.get();
            }
            return *curr;
        }
    };

    std::shared_ptr< Node > roots;

public:
    const Node& Min() const {
        if (!roots)
            throw std::logic_error("empty heap");

        return *roots;
    }

    const Node& Insert(int key, const Item& item) {
        auto n = std::make_shared< Node >(key, item);
        n->SetSelfSibling();
        auto* ret = n.get();

        Meld(n);

        return *ret;
    }

    Item ExtractMin() {

    }

    void DecreaseKey(const Node& node, int key) {
        if (node.key < key)
            throw std::logic_error("larger key provided");

        auto& n = const_cast< Node& >(node);
        n.key = key;

        if (n.IsRoot() && n.key < roots->key) {
            roots = n.shared_from_this();
            return;
        }
    }

    ~ViolationHeap() {
        if (roots)
            roots->next.reset();
    }

private:


    void Meld(std::shared_ptr< Node >& h) {
        if (!h) {
            return;
        }

        if (!roots) {
            roots = h;
            return;
        }

        if (roots->key > h->key)
            std::swap(roots, h);

        auto next = std::move(roots->next);

        roots->next = h;

        auto& curr = h->GetLast();

        curr.next = std::move(next);
    }

    // appends Node n before target Node
    void AppendBefore(std::shared_ptr< Node >& n, std::shared_ptr< Node >& target) {
        n->next = target;
    }

    void AppendAfter(std::shared_ptr< Node >& n, std::shared_ptr< Node >& target) {

    }

};

}
