#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "explicit_heap.hpp"

template < std::size_t ary >
using heap = PriorityQueues::explicit_heap< int, ary >;

using binary_heap = PriorityQueues::explicit_heap< int, 2 >;

using vector = std::vector< int >;

enum class vector_t {
    normal,
    reversed
};

vector generate_vector(std::size_t vector_size, vector_t type) {
    vector v(vector_size);
    int i = 0;
    std::generate(v.begin(), v.end(), [&i](){ return i++;});
    switch (type) {
    case vector_t::normal:
        return v;
    case vector_t::reversed:
        return vector(v.rbegin(), v.rend());
    }
}

template < typename F >
void foreach(const vector& v, F&& func) {
    std::for_each(v.begin(), v.end(), std::forward< F >(func));
}

template < std::size_t ary >
heap< ary > generate_heap(std::size_t vector_size, vector_t type) {
    heap< ary > h;
    foreach(generate_vector(vector_size, type), [&h](int i) { h.insert(i); });
    return h;
}

TEST_CASE("Normal - insert") {
    vector_t type = vector_t::normal;
    SECTION("binary") {
        std::size_t size = 10;
        auto h = generate_heap< 2 >(size, type);
        auto v = generate_vector(size, type);

        CHECK(h == v);
    }

    SECTION("4-ary"){
        auto h = generate_heap< 4 >(10, vector_t::normal);
        auto v = generate_vector(10, vector_t::normal);

        CHECK(h == v);
    }
}

TEST_CASE("Reversed - insert") {
    vector_t type = vector_t::reversed;

    SECTION("binary") {
        auto h = generate_heap< 2 >(10, type);
        vector expected = { 0, 1, 4, 3, 2, 8, 5, 9, 6, 7};

        CHECK(h == expected);
    }

    SECTION("4-ary") {
        auto h = generate_heap< 4 >(10, type);
        vector expected = {0, 2, 1, 7, 6, 9, 5, 4, 3, 8};

        CHECK(h == expected);
    }
}


TEST_CASE("Normal - extract_min") {
    vector_t type = vector_t::normal;

    SECTION("binary") {
        auto h = generate_heap< 2 >(5, type);
        int min = h.extract_min();
        vector expected = {1, 3, 2, 4};
        CHECK(min == 0);
        CHECK(h == expected);
    }

    SECTION("4-ary") {
        auto h = generate_heap< 4 >(10, type);
        vector expected = {1, 5, 2, 3, 4, 9, 6, 7, 8};

        int min = h.extract_min();

        CHECK(min == 0);
        CHECK(h == expected);
    }
}


TEST_CASE("Reversed - extract_min") {
    vector_t type = vector_t::reversed;

    SECTION("binary") {
        auto h = generate_heap< 2 >(5, type);
        vector expected = {1, 2, 3, 4};

        int min = h.extract_min();

        CHECK(min == 0);
        CHECK(h == expected);
    }

    SECTION("4-ary") {
        auto h = generate_heap< 4 >(10, type);
        vector expected = {1, 2, 8, 7, 6, 9, 5, 4, 3};

        int min = h.extract_min();

        CHECK(min == 0);
        CHECK(h == expected);
    }
}

