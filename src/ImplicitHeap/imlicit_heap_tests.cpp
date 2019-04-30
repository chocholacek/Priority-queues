#define CATCH_CONFIG_MAIN

#include "../../catch/catch.hpp"
#include "implicit_heap.hpp"

using heap = MC::ImplicitHeap<int>;


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

heap generate_heap(std::size_t vector_size, vector_t type) {
    heap h;
    foreach(generate_vector(vector_size, type), [&h](int i) { h.Insert(i, i); });
    return h;
}

bool operator==(const heap& h, const vector& v) {
    auto elems = h.Elements();
    std::vector< int > keys(elems.size());
    std::transform(elems.begin(), elems.end(), keys.begin(), [](const auto& b) { return b.key; });
    return keys == v;
}

TEST_CASE("Normal - insert") {
    vector_t type = vector_t::normal;
    SECTION("binary") {
        std::size_t size = 10;
        auto h = generate_heap(size, type);
        auto v = generate_vector(size, type);

        CHECK(h == v);
    }
}

TEST_CASE("Reversed - insert") {
    vector_t type = vector_t::reversed;

    SECTION("binary") {
        auto h = generate_heap(10, type);
        vector expected = {0, 1, 4, 3, 2, 8, 5, 9, 6, 7};

        CHECK(h == expected);
    }
}


TEST_CASE("Normal - extract") {
    vector_t type = vector_t::normal;

    SECTION("binary") {
        auto h = generate_heap(5, type);
        int min = h.ExtractMin();
        vector expected = {1, 3, 2, 4};
        CHECK(min == 0);
        CHECK(h == expected);
    }


}


TEST_CASE("Reversed - extract") {
    vector_t type = vector_t::reversed;

    SECTION("binary") {
        auto h = generate_heap(5, type);
        vector expected = {1, 2, 3, 4};

        int min = h.ExtractMin();

        CHECK(min == 0);
        CHECK(h == expected);
    }

}

TEST_CASE("Decrease Key") {
    heap h;
    std::vector< const heap::NodeType* > inserted;

    for (int i = 0 ; i < 10; ++i)
        inserted.push_back(h.Insert(i, i));

    h.DecreaseKey(inserted.back(), -1);



    REQUIRE(h.Min().key == -1);
    REQUIRE(h.Min().item == 9);
}

