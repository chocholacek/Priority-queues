#define CATCH_CONFIG_MAIN

#include <random>
#include "../../catch/catch.hpp"
#include "fibonacci_heap.hpp"


using fibonacci_heap = MC::FibonacciHeap< int >;

TEST_CASE("ctor") {
    fibonacci_heap f;

    REQUIRE_THROWS(f.Min());
}

TEST_CASE("insert") {
    fibonacci_heap f;
    for (int i = 0; i < 10; ++i) {
        f.Insert(i, i);
    }

    REQUIRE_NOTHROW(f.Min());
    const auto& m = f.Min();

    REQUIRE(m.item == 0);
    auto a = m.next;
    for (int i = 9; i > 0; --i) {
        CHECK(a->key == i);
        REQUIRE(a->next->prev == a);
        REQUIRE(a->prev->next == a);
        a = a->next;
    }
}

TEST_CASE("extract") {
    fibonacci_heap f;
    for (int i = 0; i < 4; ++i) {
        f.Insert(i, i);
    }
    for (int i = 0; i < 4; ++i) {
        std::unique_ptr< fibonacci_heap::NodeType > r;
        REQUIRE_NOTHROW(r = f.ExtractMin());
        CHECK(r->key == i);
    }
}
TEST_CASE("kyssery") {
    fibonacci_heap f;
    std::random_device r;
    std::mt19937_64 rng(r());
    std::uniform_int_distribution< int > uid;

    std::list< const fibonacci_heap::NodeType* > inserted;

    for (int i = 0; i < 100000; ++i) {
        try {
            auto op = uid(rng) % 3;
            switch (op) {
            case 0: {
                int k = uid(rng);
                int v = uid(rng);
                inserted.push_back(f.Insert(k, v));
                break;
            }
            case 1: {
                if (inserted.empty())
                    continue;
                int index = uid(rng) % inserted.size();
                auto it = inserted.begin();
                std::advance(it, index);
                auto ptr = *it;
                int k = uid(rng);
                if (k > ptr->key)
                    k = ptr->key - 1;
                f.DecreaseKey(ptr, k);
                break;
            }
            case 2: {
                auto ptr = f.ExtractMin();
                auto it  = std::find(inserted.begin(), inserted.end(), ptr.get());
                if (it != inserted.end())
                    inserted.erase(it);
                break;
            }
            }
        } catch (const std::logic_error&) {}

    }
}

