#pragma once

#include <iostream>
#include <chrono>
#include <numeric>
#include <memory>
#include <list>

namespace MC {

struct OP {
    enum class type {
        Insert,
        DecreaseKey,
        ExtractMin
    };

    type op;
    int value;
    int key;
};

template < typename T >
class Benchmark {
    std::chrono::high_resolution_clock timer;

    using ms = std::chrono::microseconds;

    ms RunOperations(const std::vector< OP >& operations) {
        using Node = typename T::NodeType;
        using uptr = std::unique_ptr< Node >;
        using std::chrono::duration_cast;

        std::list< const Node* > inserted;

        T heap;

        auto start = timer.now();
        for (auto op : operations) {
            try {

                switch (op.op) {
                case OP::type::Insert:
                    inserted.push_back(heap.Insert(op.key, op.value));
                    break;
                case OP::type::DecreaseKey: {
                    if (heap.Empty()) {
                        inserted.push_back(heap.Insert(op.key, op.value));
                        continue;
                    }

                    auto it = inserted.begin();
                    std::advance(it, op.value % inserted.size());
                    auto n = *it;
                    int k = n->key > op.key ? op.key : n->key - 1;
                    heap.DecreaseKey(n, k);
                    break;
                }
                case OP::type::ExtractMin: {
                    if (heap.Empty()) {
                        inserted.push_back(heap.Insert(op.key, op.value));
                        continue;
                    }
                    uptr p = heap.ExtractMin();
                    auto it = std::find(inserted.begin(), inserted.end(), p.get());
                    inserted.erase(it);
                    break;
                }
                }

            } catch (const std::logic_error&) {}
        }

        auto end = timer.now();

        return duration_cast< ms >(end - start);
    }

public:
    std::vector< ms > results;


    void Run(const std::vector< OP >& sequence)  {
        using std::chrono::microseconds;
        results.push_back(RunOperations(sequence));
    }

    double Average() const {
        auto total = std::accumulate(results.begin(), results.end(), 0ull, [](auto a, auto b) {
            return a + b.count();
        });

        return total / static_cast< double >(results.size());
    }

    std::string Name() const { return T().Name; }
};

}
