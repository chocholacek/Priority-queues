#define CATCH_CONFIG_MAIN
#include "../../catch/catch.hpp"
#include "explicit_heap.hpp"

using min_heap = MC::ExplicitHeap< int >;

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

min_heap generate_min_heap(std::size_t vector_size, vector_t type) {
    min_heap h;
    foreach(generate_vector(vector_size, type), [&h](int i) { h.Insert(i, i); });
    return h;
}

template < typename T >
bool operator==(const MC::ExplicitHeap< T >& h, const std::vector< T >& v) {
    auto p = MC::preorder(h);
    return p == v;
}

template < typename T >
void Equals(const MC::ExplicitHeap< T >& h, const std::vector< T >& v) {
    auto p = MC::preorder(h);
    CHECK(p == v);
}

TEST_CASE("Normal - insert") {
    vector_t type = vector_t::normal;
    SECTION("binary") {
        std::size_t size = 10;
        auto h = generate_min_heap(size, type);
        vector v = {0, 1, 3, 7, 8, 4, 9, 2, 5, 6};

        Equals(h, v);
    }

}

TEST_CASE("Reversed - insert") {
    vector_t type = vector_t::reversed;

    SECTION("binary") {
        auto h = generate_min_heap(10, type);
        vector expected = { 0, 1, 3, 9, 6, 2, 7, 4, 8, 5};

        Equals(h, expected);

    }

}

//
TEST_CASE("Normal - extract") {
    vector_t type = vector_t::normal;

    SECTION("binary") {
        auto h = generate_min_heap(5, type);
        int min = h.ExtractMin();
        vector expected = {1, 3, 4, 2};
        CHECK(min == 0);
        Equals(h, expected);
    }
}
//
//
TEST_CASE("Reversed - extract") {
    vector_t type = vector_t::reversed;

    SECTION("binary") {
        auto h = generate_min_heap(5, type);
        vector expected = {1, 2, 4, 3};

        int min = h.ExtractMin();

        CHECK(min == 0);
        Equals(h, expected);
    }
}
//
