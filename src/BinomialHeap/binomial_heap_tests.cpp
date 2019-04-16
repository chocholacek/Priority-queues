#define CATCH_CONFIG_MAIN
#include "../../catch/catch.hpp"
#include "binomial_heap.hpp"

using namespace MC;
using bh = BinomialHeap< int >;


struct test : bh {
    using node = bh::Node;
    using unique = bh::unique;

    const auto& roots() const {
        return bh::roots;
    }

    template < typename F >
    bool all(F&& func) const {
        return std::all_of(bh::roots.begin(), bh::roots.end(), std::forward< F >(func));
    }

    template < typename F >
    bool any(F&& func) const {
        return std::any_of(bh::roots.begin(), bh::roots.end(), std::forward< F >(func));
    }

    ftemplate < typename F >
    auto where(F&& func) const {
        using rw = std::reference_wrapper< const unique >;
        std::vector< rw > sat(roots().size());
        for (const auto& p : roots()) {
            if (func()(p))
                sat.emplace_back(std::cref(p));
        }
        return sat;
    }
};

using node = test::node;
using cnode = const node;
using unique = test::unique;
using cunique = const unique;

TEST_CASE("simple insert") {
    test t;
    t.Insert(1, 1);

    auto& r = t.roots();
    REQUIRE(r[0]);

    cunique& p = r[0];
    CHECK(p->degree == 0);
    CHECK(p->key == 1);
    CHECK(p->item == 1);
    CHECK(p->children.empty());
    CHECK(p->parent == nullptr);
}

TEST_CASE("multiple inserts") {
    test t;
    auto& r = t.roots();
    t.Insert(0, 0);

    INFO("2 elements") {

        t.Insert(1, 1);

        // should look like:
        // _   0
        //     |
        //     1

        REQUIRE(!r[0]);
        REQUIRE(r[1]);

        cunique& p = r[1];
        CHECK(p->degree == 1);
        CHECK(p->key == 0);

        // child check
        REQUIRE(!p->children.empty());
        cnode& n = *p->children.back();
        CHECK(n.key == 1);
        REQUIRE(n.children.empty());
        REQUIRE(n.parent == p.get());
    }

    INFO("3 elements") {
        t.Insert(2, 2);

        // should look like:
        // 2   0
        //     |
        //     1

        REQUIRE(r[0]);
        REQUIRE(r[1]);

        cunique& d0 = r[0];
        cunique& d1 = r[1];

        // degree 0
        CHECK(d0->degree == 0);
        CHECK(d0->key == 2);
        CHECK(d0->children.empty());

        // degree 1
        CHECK(d1->degree == 1);
        CHECK(d1->key == 0);

        // child check
        REQUIRE(!d1->children.empty());
        cnode& n = *d1->children.back();
        CHECK(n.key == 1);
        REQUIRE(n.children.empty());
        CHECK(n.parent == d1.get());
    }

    INFO("4 elements") {
        t.Insert(3, 3);

        // should look like this:
        // _   _  0
        //       /|
        //      1 2
        //        |
        //        3

        REQUIRE(!r[0]);
        REQUIRE(!r[1]);
        REQUIRE(r[2]);

        cunique& p = r[2];
        REQUIRE(p->degree == 2);
        CHECK(p->key == 0);

        // child check
        REQUIRE(p->children.size() == 2);
        cnode& n0 = *p->children[0];
        cnode& n1 = *p->children[1];

        // first child of '0'
        REQUIRE(n0.degree == 0);
        CHECK(n0.key == 1);
        REQUIRE(n0.children.empty());
        CHECK(n0.parent == p.get());

        // second child of '0'
        REQUIRE(n1.degree == 1);
        CHECK(n1.key == 2);
        REQUIRE(n1.children.size() == 1);
        CHECK(n1.parent == p.get());
        cnode& f = *n1.children[0];

        // child of '2'
        CHECK(f.degree == 0);
        CHECK(f.key == 3);
        CHECK(f.children.empty());
        CHECK(f.parent == &n1);
    }
}

TEST_CASE("Simple extract") {
    test t;
    t.Insert(0, 0);
    auto i = t.ExtractMin();

    REQUIRE(i == 0);
    REQUIRE(t.all([](const auto& r) { return r == nullptr; }));
}

TEST_CASE("ExtractMin") {
    test t;
    for (int i = 0; i < 4; ++i) {
        t.Insert(i, i);
    }

    // TODO
    for (int i = 0; i < 4; ++i) {
        int m = t.ExtractMin();
        CHECK(m == i);
    }
}
