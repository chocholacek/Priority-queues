#define CATCH_CONFIG_MAIN

#include "../../catch/catch.hpp"
#include "violation_heap.hpp"


using vh = MC::ViolationHeap< int >;

struct Test : protected vh {
    using Node = typename vh::Node;

    using vh::Insert;
    using vh::roots;

    vh& base;
    Test() : base(*this) {}

    const Node* Insert(int k) {
        return Insert(k, k);
    }

    const auto& Min() const {
        REQUIRE_NOTHROW(vh::Min());
        return vh::Min();
    }

    void DecreaseKey(const Node* n, int k) {
        REQUIRE_NOTHROW(vh::DecreaseKey(n, k));
    }
};

using Node = Test::Node;



TEST_CASE("Insert") {
    Test t;

    auto gm = [&t]() -> const Node & {
        return t.Min();
    };

    SECTION("simple - one item") {
        auto n = t.Insert(0);

        REQUIRE(n->key == 0);

        auto &min = gm();

        CHECK(min.key == 0);
        REQUIRE(min.next == &min);
        REQUIRE(min.prev == nullptr);
        REQUIRE(min.child == nullptr);
    }



    SECTION("multiple - two items lesser first") {
        auto n = t.Insert(0);
        auto m = t.Insert(1);

        REQUIRE(n->next == m);
        REQUIRE(m->next == n);

        CHECK(n->key == 0);
        CHECK(m->key == 1);

        auto& min = gm();
        CHECK(min.key == 0);
    }

    SECTION("multiple - two items larger first") {
        auto m = t.Insert(1);
        auto n = t.Insert(0);

        REQUIRE(n->next == m);
        REQUIRE(m->next == n);

        CHECK(n->key == 0);
        CHECK(m->key == 1);

        auto& min = gm();
        CHECK(min.key == 0);
    }
}

TEST_CASE("DecreaseKey") {
    Test t;
    auto f = t.Insert(5);

    SECTION("single element - invalid key") {
        REQUIRE_THROWS_AS(t.base.DecreaseKey(&t.Min(), 10), std::logic_error);
    }

    SECTION("single element - valid key") {
        t.DecreaseKey(f, 0);

        CHECK(t.Min().key == 0);
    }

    SECTION("multiple elements - no change") {
        auto s = t.Insert(10);

        t.DecreaseKey(s, 8);

        auto& min = t.Min();

        REQUIRE(min.key == 5);
        REQUIRE(min.next->key == 8);
        REQUIRE(min.next->next == &min);
    }

    SECTION("multiple elements - new min") {
        auto s = t.Insert(10);

        t.DecreaseKey(s, 0);

        auto& min = t.Min();
        REQUIRE(min.key == 0);
        REQUIRE(min.next->key == 5);
        REQUIRE(min.next->next == &min);
    }

}

TEST_CASE("ExtractMin") {
    SECTION("Simple") {
        Test t;
        auto n = t.Insert(0);
        int i = t.base.ExtractMin();
        REQUIRE_THROWS(t.base.Min());
        CHECK(i == 0);
    }

    Test t;
    t.Insert(0);
    t.Insert(1);
    t.Insert(2);


    SECTION("Complicated - 3 elements") {
        int i = t.base.ExtractMin();
        CHECK(i == 0);

        auto& m = t.Min();
        REQUIRE(m.key == 1);
        REQUIRE(m.next != nullptr);
        CHECK(m.next->key == 2);
        REQUIRE(m.next->next == &m);
        REQUIRE(m.prev == nullptr);
        REQUIRE(m.child == nullptr);
    }

    SECTION("Complicated - 4 elements") {
        t.Insert(3);

        int i = t.base.ExtractMin();
        CHECK(i == 0);

        auto&m = t.Min();
        REQUIRE(m.key == 1);
        REQUIRE(m.child != nullptr);
        auto c = m.child;
        CHECK(c->key == 3);
        REQUIRE(c->prev == &m);
        REQUIRE(c->next != nullptr);
        REQUIRE(c->child == nullptr);


        auto c2 = c->next;
        CHECK(c2->key == 2);
        REQUIRE(c2->prev == c);
        REQUIRE(c2->next == nullptr);
        REQUIRE(c2->child == nullptr);
    }
}
