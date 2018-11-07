#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "fibonnaci_heap.hpp"

using namespace PriorityQueues;

TEST_CASE("ctor") {
//    fibonacci_heap f;
//    REQUIRE(f._count == 0);
//    REQUIRE(f._min == nullptr);
}

TEST_CASE("insert") {
    fibonacci_heap f;
    for (int i = 0; i < 10; ++i) {
        f.insert(i);
    }

//    REQUIRE(f._count == 10);
//    REQUIRE(f._min->value == 0);
//    fibonacci_heap::node* a = f._min->next;
//    for (int i = 9; i > 0; --i) {
//        CHECK(a->value == i);
//        REQUIRE(a->next->prev == a);
//        REQUIRE(a->prev->next == a);
//        a = a->next;
//    }
}

TEST_CASE("extract") {
    fibonacci_heap f;
    for (int i = 0; i < 4; ++i) {
        f.insert(i);
    }
    for (int i = 0; i < 4; ++i) {
        f.extract_min();
        //CHECK(f.extract_min() == i);
    }

}

//using fib = PriorityQueues::fibonacci_heap< int >;
//
//struct test : fib {
//    using fib::node;
//    using fib::shared_ptr;
//};
//
//using node = test::node;
//using shared = test::shared_ptr;
//
//
//TEST_CASE("node make")
//{
//    shared n;
//    REQUIRE_NOTHROW(n = node::make(42));
//    REQUIRE(n->key == 42);
//    REQUIRE(n->right == n);
//    REQUIRE(n->left.lock() == n);
//}
//
//TEST_CASE("node add_right") {
//    auto n = node::make(0);
//    n->add_right(node::make(1));
//
//    REQUIRE(n->right->right == n);
//    REQUIRE(n->left.lock() == n->right);
//    REQUIRE(n->right->left.lock() == n);
//    REQUIRE(n->right->key == 1);
//}
//
//TEST_CASE("node remove self") {
////    auto n = node::make();
////    n->remove_self();
//}
//
//TEST_CASE("heap insert") {
//    fib f;
//    f.insert(0);
//    REQUIRE()
//}

