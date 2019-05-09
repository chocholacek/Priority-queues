
#include <ImplicitHeap/implicit_heap.hpp>
#include <ExplicitHeap/explicit_heap.hpp>
#include <FibonacciHeap/fibonacci_heap.hpp>
#include <BinomialHeap/binomial_heap.hpp>
#include <ViolationHeap/violation_heap.hpp>
#include <RankPairingHeap/rp_heap.hpp>

#include <functional>
#include <random>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>

using std::ofstream;
using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

using TItem = int;
using Implicit = MC::ImplicitHeap< TItem >;
using Explicit = MC::ExplicitHeap< TItem >;
using Fibonacci = MC::FibonacciHeap< TItem >;
using Binomial = MC::BinomialHeap< TItem >;
using Violation = MC::ViolationHeap< TItem >;
using RPHeap = MC::RankPairingHeap< TItem >;

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

ofstream file("wallclock_results.json");
high_resolution_clock timer;
std::vector< OP > operations;

std::random_device r;
std::mt19937_64 rng(r());
std::uniform_int_distribution< TItem > uid(-32, 32);

std::string indent(int i) {
    return std::string(i, '\t');
}


void AppendToFile(const std::string& line, int i) {
    file << indent(i) << line << std::endl;
}

void AppendToFile(const std::vector< microseconds >& results, int i) {
    file << indent(i) << "\"results\": [";
    for (unsigned i = 0; i < results.size(); ++i) {
        file << results[i].count();
        if (i != results.size() - 1)
            file << ", ";
    }
    file << "]," << std::endl;
}

void GenerateRandomSequence(int size) {
    operations.resize(size);
    for (auto& op : operations) {

        op.op = static_cast< OP::type >((unsigned)uid(rng) % (sizeof(OP::type) - 1));
        op.value = uid(rng);
        op.key = uid(rng);
    }
}

std::map< OP::type , std::string > opmap = {
        {OP::type::Insert, "I"},
        {OP::type::DecreaseKey, "D"},
        {OP::type::ExtractMin, "E"}
};

void LogOperations() {
    ofstream log("last.txt");
    for (auto op : operations) {
        auto c = opmap[op.op];
        if (c.empty())
            c = "U" + std::to_string(static_cast< int >(op.op));
        log <<  c + ":" + std::to_string(op.key) + ":" + std::to_string(op.value) << std::endl;
    }
}

template < typename T >
microseconds RunOperations() {
    using Node = typename T::NodeType;
    using uptr = std::unique_ptr< Node >;

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
                if (inserted.empty()) {
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
                uptr p = heap.ExtractMin();
                auto it = std::find(inserted.begin(), inserted.end(), p.get());
                inserted.erase(it);
                break;
            }
            }

        } catch (const std::logic_error&) {}
    }

    auto end = timer.now();

    return duration_cast< microseconds >(end - start);
}

template < typename T >
void Benchmark(int iterations, int sequence, bool log = false) {
    std::vector< microseconds > results(iterations);
    auto n = T().Name;

    std::cout << "Benchmark - " << n << ": ";
    std::flush(std::cout);

    auto start = timer.now();


    AppendToFile("\"" + n + "\": {", 1);

    for (int i = 0; i < iterations; ++i) {
        GenerateRandomSequence(sequence);
        if (log)
            LogOperations();
        results[i] = RunOperations< T >();
    }
    AppendToFile(results, 2);

    auto accum = std::accumulate(results.begin(), results.end(), 0.0, [](auto a, auto b) {
        return a + b.count();
    });

    AppendToFile("\"average\": " + std::to_string(accum / results.size()), 2);
    AppendToFile("},", 1);

    auto end = timer.now();
    std::cout << duration_cast< milliseconds >(end - start).count() / 1000.0 << " s" << std::endl;
}


int main() {

    int runs = 10;
    int sequence_size = 100000;

    AppendToFile("{", 0);
//    Benchmark< Implicit >(runs, sequence_size);
//    Benchmark< Explicit >(runs, sequence_size);
    Benchmark< Fibonacci >(runs, sequence_size);
//    Benchmark< Binomial >(runs, sequence_size);
    Benchmark< Violation >(runs, sequence_size);
//    Benchmark< RPHeap >(runs, sequence_size);
    AppendToFile("}", 0);
    return 0;
}
