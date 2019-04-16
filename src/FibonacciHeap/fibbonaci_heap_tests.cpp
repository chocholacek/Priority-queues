#define CATCH_CONFIG_MAIN
#include "../../catch/catch.hpp"
#include "fibonacci_heap.hpp"


using fibonacci_heap = PriorityQueues::FibonacciHeap< int >;

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
        int r;
        REQUIRE_NOTHROW(r = f.ExtractMin());
        CHECK(r == i);
    }
}
TEST_CASE("kyssery") {
    fibonacci_heap f;
//    f.insert(0, 0);
//    f.insert(1, 1);
//
//    f.extract_min();
//    f.extract_min();
}

