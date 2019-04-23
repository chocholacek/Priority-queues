#define BRICK_BENCHMARK_REG
#define BRICK_BENCHMARK_MAIN

/* ./bench category:stack | gnuplot > soubor.pdf */

#include <brick-benchmark>
#include <ImplicitHeap/implicit_heap.hpp>

#include <functional>
#include <random>


using namespace brick;
using ImplicitHeap = MC::ImplicitHeap< int >;

struct Wallclock : benchmark::Group {
    Wallclock() : rng(r()) {
        x.type = benchmark::Axis::Quantitative;
        x.name = "Elements";
        x.min = 1000;
        x.max = 10000;

        y.type = benchmark::Axis::Qualitative;
        y.name = "heap";
        y.min = 1;
        y.max = 1;

        y._render = [](int i) {
            switch (i) {
            case 1: return "binary (implicit)";
            case 2: return "binary (explicit)";
            }
        };
    }

//    struct OP {
//        enum Type {
//
//        };
//
//        Type type;
//        int what;
//        int to;
//    };

    enum class OP {
        Insert,
        Extract,
        Decrease
    };

    void setup(int _p, int _q) override {
        p = _p; q = _q;

        rand = GenerateOperationSequence(p);
    }


    BENCHMARK(random) {
        switch (q) {
        case 1:  Random< ImplicitHeap >();
        }
    }

    template < typename H >
    void Random() {
        H h;

        using Node = typename H::Node;
        using RW = std::reference_wrapper< Node >;
        std::vector< RW > data;

        for (auto op : rand) {
            switch (op) {
            case OP::Insert: {
                int num = uid(rng);
                data.push_back(std::cref(h.Insert(num, num)));
            }
            }
        }
    }

    std::vector< OP > GenerateOperationSequence(int size) {
        std::vector< OP > ret(size);

        for (auto& op : ret) {
            op = static_cast< OP >(uid(rng) % 3);
        }

        return ret;
    }

    std::random_device r;

    std::mt19937_64 rng;

    std::uniform_int_distribution< int > uid;

    std::vector< OP > rand;
};