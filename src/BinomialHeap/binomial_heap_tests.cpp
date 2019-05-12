#define CATCH_CONFIG_MAIN
#include "../../catch/catch.hpp"
#include "binomial_heap.hpp"

using namespace MC;
using bh = BinomialHeap< int >;

using N = bh::NodeType;

struct test : protected bh {
    using Handle = const bh::NodeType*;
    const auto& Min() {
        REQUIRE_NOTHROW(bh::Min());
        return bh::Min();
    }

    const auto Insert(int v) {
        return bh::Insert(v, v);
    }

    void DK(Handle h, int k) {
        REQUIRE_NOTHROW(bh::DecreaseKey(h, k));
    }

    auto Ex() {
        std::unique_ptr< bh::NodeType > ret;
        REQUIRE_NOTHROW(ret = std::move(bh::ExtractMin()));
        return ret;
    }
};

using Handle = test::Handle;

TEST_CASE("simple insert") {
    test t;
    auto p = t.Insert(1);
    auto& i = t.Min();

    REQUIRE(p->key == i.key);

    CHECK(p->Degree() == 0);
    CHECK(p->key == 1);
    CHECK(p->item == 1);
    CHECK(p->children.empty());
    CHECK(p->parent == nullptr);
}
//
//template < typename N >
//void CheckPrevNext(const N* n, const N* prev = nullptr, const N* next = nullptr) {
//    CHECK(n->prev == prev);
//    CHECK(n->next == next);
//}
//
TEST_CASE("multiple inserts") {
    test t;
    t.Insert(0);

    INFO("2 elements")
    {
        auto p = t.Insert(1);

        // should look like:
        // 0
        // |
        // 1

        const auto &i = t.Min();

        CHECK(i.degree == 1);
        CHECK(i.key == 0);

        // child check
        REQUIRE(i.child != nullptr);
        auto n = i.child;
        CHECK(n->key == 1);
        REQUIRE(n->child == nullptr);
        REQUIRE(n->parent != nullptr);
        REQUIRE(n->parent->key == 0);
    }
}
//
//
//    INFO("3 elements") {
//        t.Insert(2);
//
//        // should look like:
//        // 0   2
//        // |
//        // 1
//
//        const auto& p = t.Min();
//        auto n = p.next;
//
//        // degree 0
//        CHECK(n->degree == 0);
//        CHECK(n->key == 2);
//        CHECK(n->child == nullptr);
//
//        // degree 1
//        CHECK(p.degree == 1);
//        CHECK(p.key == 0);
//
//        // child check
//        REQUIRE(p.child != nullptr);
//        auto c = p.child;
//        CHECK(c->key == 1);
//        REQUIRE(c->child == nullptr);
//        CHECK(c->parent->key == 0);
//    }
//
//    INFO("4 elements") {
//        t.Insert(3);
//
//        // should look like this:
//        //   0
//        //  /
//        // 2 - 1
//        // |
//        // 3
//
//        const auto& p = t.Min();
//
//        REQUIRE(p.degree == 2);
//        CHECK(p.key == 0);
//        CheckPrevNext(&p);
//
//        REQUIRE(p.child != nullptr);
//        auto c1 = p.child;
//        REQUIRE(c1->next);
//        auto c2 = c1->next;
//
//        REQUIRE(c1->key == 2);
//        REQUIRE(c2->key == 1);
//    }
//
//}
//
//TEST_CASE("DecreaseKey") {
//    test t;
//    std::vector< Handle > inserts;
//    for (int i = 0; i < 8; ++i)
//        inserts.push_back(t.Insert(i));
//
//    t.DK(inserts.back(), -1);
//}
//
//TEST_CASE("Extract") {
//    test t;
//    std::vector< Handle > inserts;
//    for (int i = 0; i < 8; ++i)
//        inserts.push_back(t.Insert(i));
//    t.Ex();
//}
//
///*
//TEST_CASE("Simple extract") {
//    test t;
//    t.Insert(0, 0);
//    auto i = t.ExtractMin();
//
//    REQUIRE(i == 0);
//    REQUIRE(t.all([](const auto& r) { return r == nullptr; }));
//}
//
//TEST_CASE("ExtractMin") {
//    test t;
//    for (int i = 0; i < 4; ++i) {
//        t.Insert(i, i);
//    }
//
//    // TODO
//    for (int i = 0; i < 4; ++i) {
//        int m = t.ExtractMin();
//        CHECK(m == i);
//    }
//}
//*/