#pragma once

#include "../base/HeapBase.hpp"
#include <memory>
#include <functional>
#include <vector>


namespace MC {

template < typename Item >
class ImplicitHeap : public HeapBase {
    struct Node {
        int key;
        Item item;
        int index;

        Node(int k, const Item& i, int index)
                : key(k), item(i), index(index) {}
    };

    // TODO fix
    std::vector< std::unique_ptr< Node > > array;


    int ParentIndex(int index) const { return (index - 1) / 2 ; }

    int LeftIndex(int index) const { return index * 2 + 1; }

    int RightIndex(int index) const { return index * 2 + 2; }


    Node& Dependent(int i, std::function< int(int) > indexer) const {
        return array[indexer(i)];
    }

    std::unique_ptr< Node >& Parent(int index) {
        return array[ParentIndex(index)];
    }

    std::unique_ptr< Node >& Left(int index) {
        return array[LeftIndex(index)];
    }

    std::unique_ptr< Node >& Right(int index) {
        return array[RightIndex(index)];
    }

    void HeapifyDown(int index) {
        if (index >= array.size())
            return;

        int smallest = index;

        if (InBounds(LeftIndex(index)) && Left(index)->key < array[index]->key) {
            smallest = LeftIndex(index);
        }

        if (InBounds(RightIndex(index)) && Right(index)->key < array[smallest]->key) {
            smallest = RightIndex(index);
        }

        if (smallest != index) {
            std::swap(array[index], array[smallest]);
            std::swap(array[index]->index, array[smallest]->index);
            HeapifyDown(smallest);
        }
    }

    bool InBounds(int index) const { return index < array.size(); }

public:
    using NodeType = typename ImplicitHeap::Node;

    ImplicitHeap() : HeapBase("Implicit heap") {
        array.reserve(64);
    }

    const Node& Min() const {
        if (array.empty())
            EmptyException();
        return *array[0];
    }

    void DecreaseKey(const Node* node, int key) {
        DecreaseKey(node->index, key);
    }

    const Node* Insert(int key, const Item& item) {
        array.emplace_back(std::make_unique< Node >(Infinity, item, array.size()));
        return array[DecreaseKey(array.size() - 1, key)].get();
    }


    std::unique_ptr< Node > ExtractMin() {
        if (array.empty())
            EmptyException();

        std::swap(array.front(), array.back());
        std::swap(array.front()->index, array.back()->index);
        auto ret = std::move(array.back());
        array.pop_back();
        HeapifyDown(0);
        return ret;
    }

    const std::vector< std::unique_ptr< Node > >& Elements() const { return array; }

private:
    int DecreaseKey(int index, int key) {
        if (key > array[index]->key)
            InvalidKeyException();

        array[index]->key = key;

        while (index > 0 && array[index]->key < Parent(index)->key) {
            std::swap(array[index], Parent(index));
            std::swap(array[index]->index, Parent(index)->index);
            index = ParentIndex(index);
        }

        return index;
    }
};

}