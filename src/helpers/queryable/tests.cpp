#define CATCH_CONFIG_MAIN

#include "../../../catch/catch.hpp"
#include "queryable.hpp"

using vector_t = std::vector< int >;
using q_int = randoms::queryable< int >;

template < typename T, typename F >
std::vector< T > data(unsigned count, F& factory) {
    std::vector< T > ret(count);
    for (int i = 0; i < count; ++i) {
        ret.emplace_back(factory());
    }
    return ret;
}

template < typename T >
auto query(const std::vector< T >& v) {
    return randoms::as_queryable< T >(v);
}

TEST_CASE("constructible") {
    SECTION("from vector") {
        vector_t v;
        for (int i = 0; i < 10; ++i) {
            v.push_back(i);
        }

        auto q = randoms::as_queryable(v);
    }
}

TEST_CASE("indexer") {
    SECTION("simple types") {
        SECTION("int") {
            int i = 0;
            auto f = [&i](){ return i++;};
            auto v = data< int >(10, f);

            auto q = query(v);

            i = 0;
            for (int x : v) {
                CHECK(x == q[i++]);
            }
        }
    }
}

TEST_CASE("where") {
    int i = 0;
    auto f = [&i](){ return i++;};
    auto v = data< int >(10, f);

    auto q = query(v);

    SECTION("simple") {
        auto res = q.where([](const int& i){ return i % 2 == 0; });
    }
}

TEST_CASE("as queryable") {
    std::array< int, 1 > a = { 0 };

    std::vector< std::unique_ptr< int > > v = {
            std::make_unique< int >(0)
    };

    auto q = randoms::as_queryable(v);

    //auto q = randoms::as_queryable(a);
}

