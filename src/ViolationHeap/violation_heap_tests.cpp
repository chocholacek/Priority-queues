#define CATCH_CONFIG_MAIN

#include "../../catch/catch.hpp"

#include "violation_heap.hpp"

using vh = MC::ViolationHeap< int >;

struct Test : vh {
    using Node = typename vh::Node;
};

using Node = Test::Node;

TEST_CASE("constructible") {
    vh h;
}

//TEST_CASE("insert") {
//    vh h;
//
//    SECTION("simple") {
//        h.Insert(0, 0);
//        REQUIRE_NOTHROW(h.Min());
//        auto& min = h.Min();
//        CHECK(min.item == 0);
//
//        // require same address of next for circularity
//        REQUIRE(min.next.get() == &min);
//    }
//
//    SECTION("multiple - larger key") {
//        h.Insert(0, 0);
//        h.Insert(1, 1);
//
//        REQUIRE_NOTHROW(h.Min());
//        auto& min = h.Min();
//        CHECK(min.item == 0);
//
//        REQUIRE(min.next != nullptr);
//        auto& next = *min.next;
//        CHECK(next.key == 1);
//
//        REQUIRE(next.next.get() == &min);
//    }
//
//    SECTION("multiple - lesser key") {
//        h.Insert(1, 1);
//        h.Insert(0, 0);
//
//
//        REQUIRE_NOTHROW(h.Min());
//        auto& min = h.Min();
//        CHECK(min.item == 0);
//
//        REQUIRE(min.next != nullptr);
//        auto& next = *min.next;
//        CHECK(next.key == 1);
//
//        REQUIRE(next.next.get() == &min);
//    }
//}


const auto& Insert(vh& h, int i) {
    auto& n = h.Insert(i, i);
    REQUIRE_NOTHROW(h.Min());
    return n;
}

TEST_CASE("Insert") {
    vh h;

    auto CI = [&h](int i) -> const Node& {
        return Insert(h, i);
    };

    SECTION("simple") {

        auto& n = CI(0);
        auto& min = h.Min();

        REQUIRE(&n == &min);
        CHECK(n.item == 0);
        CHECK(n.rank == 0);

        REQUIRE(n.next.get() == &n);
    }

    SECTION("multiple - lesser first") {
        auto& n = CI(0);
        auto& m = CI(1);

        auto& min = h.Min();

        REQUIRE(&n == &min);
        CHECK(n.item == 0);

        REQUIRE(min.next.get() == &m);
        CHECK(m.item == 1);

        REQUIRE(m.next.get() == &n);
    }

    SECTION("multiple - larger first") {
        auto& n = CI(1);
        auto& m = CI(0);

        auto& min = h.Min();

        REQUIRE(&m == &min);
        CHECK(m.item == 0);

        REQUIRE(min.next.get() == &n);
        CHECK(n.item == 1);

        REQUIRE(n.next.get() == &m);
    }
}

TEST_CASE("DecreaseKey") {
    vh h;
    auto CI = [&h](int i) -> const Node& {
        return Insert(h, i);
    };

    auto& f = CI(0);

    SECTION("invalid key") {
        REQUIRE_THROWS(h.DecreaseKey(f, 1));
    }



    SECTION("only root - one root") {
        REQUIRE_NOTHROW(h.DecreaseKey(f, -1));
        CHECK(f.key == -1);
        CHECK(h.Min().key == -1);
    }

    SECTION("only root - multiple roots") {

    }
}