#define CATCH_CONFIG_MAIN

#include "../../catch/catch.hpp"
#include "rp_heap.hpp"
#include <random>

using RP = MC::RankPairingHeap< int >;

struct Test : protected RP {
public:
    using RP::Node;

    const Node& Min() const {
        REQUIRE_NOTHROW(RP::Min());
        return RP::Min();
    }

    const Node& Insert(int v) {
        return RP::Insert(v, v);
    }

    ItemType ExtractMin() {
        ItemType i;
        REQUIRE_NOTHROW(i = RP::ExtractMin());
        return i;
    }

};

std::vector< int > Random(std::size_t size) {
    std::vector< int > v(size);

    std::random_device d;
    std::mt19937_64 rng(d());
    std::uniform_int_distribution< int > uid;

    for (int& i : v) {
        i = uid(rng);
    }

    return v;
}

std::vector< int > CopyAndSort(std::vector< int > v) {
    std::sort(v.begin(), v.end());
    return v;
}


TEST_CASE("Insert") {
    Test t;

    t.Insert(0);

    auto m = t.Min();

    REQUIRE(m.key == 0);
    REQUIRE(m.next->key == m.key);
}



TEST_CASE("ExtractMin") {
    Test t;

    for (int i = 0; i < 5; ++i)
        t.Insert(i);
    for (int i = 0; i < 5; ++i)
        REQUIRE(t.ExtractMin() == i);
}

auto InsertRange = [](auto& t, const auto& v) {
    std::for_each(v.begin(), v.end(), [&t](const auto& i) {
        t.Insert(i);
    });
};

auto ExtractCheckRange = [](auto& t, const auto& v) {
    std::for_each(v.begin(), v.end(), [&t](const auto& i) {
        CHECK(t.ExtractMin() == i);
    });
};

TEST_CASE("ExtractMin - random values") {
    Test t;
    auto IR = [&t](const auto& v) {
        InsertRange(t, v);
    };

    auto CR = [&t](const auto& v) {
        ExtractCheckRange(t, v);
    };

    auto v = Random(1000);
    auto ord = CopyAndSort(v);

    IR(v);
    CR(ord);
}