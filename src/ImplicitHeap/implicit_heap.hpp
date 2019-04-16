#pragma once

#include "../../base/HeapBase.hpp"
#include <memory>
#include <functional>


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

    std::vector< Node > array;



    int ParentIndex(int index) const { return (index - 1) / 2 ; }

    int LeftIndex(int index) const { return index * 2 + 1; }

    int RightIndex(int index) const { return index * 2 + 2; }


    Node& Dependent(int i, std::function< int(int) > indexer) const {
        return array[indexer(i)];
    }

    Node& Parent(int index) {
        return array[ParentIndex(index)];
    }

    Node& Left(int index) {
        return array[LeftIndex(index)];
    }

    Node& Right(int index) {
        return array[RightIndex(index)];
    }

    void HeapifyDown(int index) {
        if (index >= array.size())
            return;

        int smallest = index;

        if (InBounds(LeftIndex(index)) && Left(index).key < array[index].key) {
            smallest = LeftIndex(index);
        }

        if (InBounds(RightIndex(index)) && Right(index).key < array[smallest].key) {
            smallest = RightIndex(index);
        }

        if (smallest != index) {
            std::swap(array[index], array[smallest]);
            HeapifyDown(smallest);
        }
    }

    bool InBounds(int index) const { return index < array.size(); }

public:
    const Node& Min() const { return array.at(0); }

    void DecreaseKey(int index, int key) {
        if (key > array[index].key)
            throw;

        array[index].key = key;

        while (index > 0 && array[index].key < Parent(index).key) {
            std::swap(array[index], Parent(index));
            index = ParentIndex(index);
        }
    }

    void DecreaseKey(const Node& node, int key) {
        DecreaseKey(node.index, key);
    }

    void Insert(int key, const Item& item) {
        array.emplace_back(Infinity, item, array.size());
        DecreaseKey(array.back(), key);
    }

    Item ExtractMin() {
        auto item = Min().item;
        std::swap(array.front(), array.back());
        array.pop_back();
        HeapifyDown(0);
        return item;
    }

    std::vector< Node > Elements() const { return array; }
};

}