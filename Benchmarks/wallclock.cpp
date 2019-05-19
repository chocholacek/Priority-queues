
#include <ImplicitHeap/implicit_heap.hpp>
#include <ExplicitHeap/explicit_heap.hpp>
#include <FibonacciHeap/fibonacci_heap.hpp>
#include <BinomialHeap/binomial_heap.hpp>
#include <ViolationHeap/violation_heap.hpp>
#include <RankPairingHeap/rp_heap.hpp>
#include <RankPairingHeap/rp_heap_t2.hpp>

#include <functional>
#include <random>
#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include "benchmark.hpp"

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
using RPHeap1 = MC::RankPairingHeap< TItem >;
using RPHeap2 = MC::RankPairingHeap2< TItem >;

using MC::Benchmark;
using MC::OP;


std::random_device r;
std::mt19937_64 rng(r());
std::uniform_int_distribution< TItem > uid(-32, 32);


std::vector< OP > GenerateRandomSequence(int size) {
    std::vector< OP > operations(size);
    for (auto& op : operations) {

        op.op = static_cast< OP::type >((unsigned)uid(rng) % (sizeof(OP::type) - 1));
        op.value = uid(rng);
        op.key = uid(rng);
    }
    return operations;
}

std::vector< OP > GenerateRandomInserts(int size) {
    std::vector< OP > operations(size);
    for (auto& op : operations) {

        op.op = OP::type::Insert;
        op.value = uid(rng);
        op.key = uid(rng);
    }
    return operations;
}

std::vector< OP > GenerateRandomSorting(int size) {
    auto inserts = GenerateRandomInserts(size);
    for (int i = 0; i < size; ++i) {
        OP op;
        op.op = OP::type::ExtractMin;
        inserts.push_back(op);
    }

    return inserts;
}

std::vector< OP > GenerateRandomInsertsDKs(int size) {
    std::vector< OP > operations(size);
	for (auto& op : operations) {
        op.op = static_cast< OP::type >((unsigned)uid(rng) % (sizeof(OP::type) - 2));
        op.value = uid(rng);
        op.key = uid(rng);
    }
    return operations;
}

std::vector< OP > GenerateDKsWithNewMin(int size) {
    auto half = size / 2;
    std::vector< OP > operations(size);
    for (int i = 0; i < half; ++i) {
        OP op;
        op.op = OP::type::Insert;
        op.value = uid(rng);
        op.key = std::abs(uid(rng));
        operations[i] = op;
    }
    for (int i = half; i < size; ++i) {
        OP op;
        op.op = OP::type::DecreaseKey;
        op.value = uid(rng);
        op.key = -i;
        operations[i] = op;
    }
    return operations;
}


std::map< std::string, double > averages;

void NormalizeAverages() {
    double min = std::min_element(
            averages.begin(),
            averages.end(),
            [](const auto& it1, const auto& it2) {
                return it1.second < it2.second;
            })->second;

    for (auto kv : averages) {
        std::cout << kv.first << ": " << kv.second / min << std::endl;
    }
}

void PrintAverages() {
    for (auto kv : averages) {
        std::cout << kv.first << ": " << kv.second << std::endl;
    }
}

void Log(const std::vector< OP >& ops) {
    std::ofstream last("last.txt");
    for (auto op : ops) {
        std::string t;
        switch (op.op) {
        case OP::type::Insert:
            t = "I";
            break;
        case OP::type::ExtractMin:
            t = "E";
            break;
        case OP::type::DecreaseKey:
            t = "D";
            break;
        }
        last << t << ":" << op.key << ":" <<  op.value << std::endl;
    }
}

template < typename Gen >
void Run(int runs, int size, Gen g, bool log = false) {
    Benchmark< Implicit > imp;
    Benchmark< Explicit > exp;
    Benchmark< Binomial > bin;
    Benchmark< Fibonacci > fib;
    Benchmark< Violation > vio;
    Benchmark< RPHeap1 > rp1;
    Benchmark< RPHeap2 > rp2;

    for (int i = 0; i < runs; ++i) {
        auto seq = g(size);
        if (log)
            Log(seq);

        imp.Run(seq);
        exp.Run(seq);
        bin.Run(seq);
        fib.Run(seq);
        vio.Run(seq);
        rp1.Run(seq);
        rp2.Run(seq);
    }

    averages[imp.Name()] = imp.Average();
    averages[exp.Name()] = exp.Average();
    averages[bin.Name()] = bin.Average();
    auto f = fib.Name();
    std::transform(f.begin(), f.end(), f.begin(), [](char c) { return std::tolower(c) ;});
    averages[f] = fib.Average();
    averages[rp1.Name()] = rp1.Average();
    averages[rp2.Name()] = rp2.Average();
    averages[vio.Name()] = vio.Average();
}


int main() {

    int runs = 100;
    int sequence_size = 100;

    std::cout << "Sequence size: " << sequence_size << ", runs: " << runs << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    Run(runs, sequence_size, GenerateRandomSequence);
    std::cout << "Average:" << std::endl;
    PrintAverages();
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "Normalized average:" << std::endl;
    NormalizeAverages();
    std::cout << "--------------------------------------------------------" << std::endl;
    return 0;
}
