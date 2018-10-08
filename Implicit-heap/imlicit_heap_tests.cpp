#define CATCH_CONFIG_MAIN

#include "../catch.hpp"
#include "implicit_heap.hpp"


template < std::size_t ary >
using heap = PriorityQueues::implicit_heap< int, ary, std::less< int > >;

TEST_CASE("construction") {
    heap< 2 > h;
}

TEST_CASE("insert") {
    SECTION("simple") {
        heap< 2 > h;
        h.insert(42);
        CHECK(42 == h.extract());
    }


    SECTION("multiple") {
        heap< 2 > h;
        for (int i = 0; i < 10; ++i)
            h.insert(i);

        for (int i = 0; i < 10; ++i) {
            CHECK(i == h.extract());
        }
    }
}