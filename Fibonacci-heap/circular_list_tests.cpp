#define CATCH_CONFIG_MAIN
#include "../catch.hpp"
#include "circular_list.hpp"

using ll = PriorityQueues::circular_list< int >;

TEST_CASE("creation") {
    ll list;
}

TEST_CASE("insert") {
    SECTION("one element") {
        ll list;
        list.insert(0);

        CHECK(list.root->value == 0);
        
        CHECK(list.root == list.root->right);
        
        CHECK(list.root == list.root->left.lock());

    }

    SECTION("more elements") {
        ll list;
        int size = 10;

        for (int i = 0; i < size; ++i)
            list.insert(i);

        std::vector< int > expected = {0, 9, 8, 7, 6, 5, 4, 3, 2, 1};
        auto ptr = list.root;
        for (int i = 0; i < size; ++i, ptr = ptr->right) {
            int l = i == 0 ? size - 1 : i - 1;
            int r = i == size - 1 ? 0 : i + 1;
            CHECK(ptr->value == expected[i]);
            CHECK(ptr->right->value == expected[r]);
            CHECK(ptr->left.lock()->value == expected[l]);
        }
    }

//    SECTION("reversed elements") {
//        ll list;
//        int size = 9;
//
//        for (int i = size; i >= 0; --i)
//            list.insert(i);
//
//        CHECK(list.root() == list.root());
//        CHECK(list.root()->value == 0);
//
//
//        auto ptr = list.first();
//        for (int i = size; i >= 0; --i, ptr = ptr->right) {
//            CHECK(ptr->value == i);
//            CHECK(ptr->right->value == (i == 0 ? size : ptr->value - 1));
//            CHECK(ptr->left.lock()->value == (i == size ? 0 : ptr->value + 1));
//        }
//
//    }
}

TEST_CASE("move") {
    ll l1, l2;
    int i = 0;

    for (; i < 2; ++i)
        l1.insert(i);

    for (; i < 5; ++i)
        l2.insert(i);

    l1.insert(std::move(l2));

    CHECK(!l2.root);

    auto begin = l1.root;
    auto ptr = l1.root;

    std::vector< int > expected = {0, 2, 4, 3, 1};
    i = 0;

    do {
        int l = i == 0 ? expected.size() - 1 : i - 1;
        int r = i == expected.size() - 1 ? 0 : i + 1;
        CHECK(ptr->value == expected[i++]);
        CHECK(ptr->left.lock()->value == expected[l]);
        CHECK(ptr->right->value == expected[r]);
        ptr = ptr->right;
    } while (begin != ptr);
}