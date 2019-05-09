#include <ViolationHeap/violation_heap.hpp>
#include <FibonacciHeap/fibonacci_heap.hpp>
#include <BinomialHeap/binomial_heap.hpp>

#include <list>
#include <algorithm>
#include <sstream>

using vh = MC::ViolationHeap< int >;
using fib = MC::FibonacciHeap< int >;
using bin = MC::BinomialHeap< int >;

template < typename T >
void f() {
    std::string out = "I:21:-28\n"
            "I:-8:-17\n"
            "I:22:-26\n"
            "I:12:1\n"
            "E:32:-28\n"
            "D:-21:-15\n"
            "D:-8:-22\n"
            "E:1:-13\n"
            "D:15:8\n"
            "E:-25:-18";

    std::stringstream ss(out);

    T h;
    using N = typename T::NodeType;
    std::list< const N* > inserted;

    auto insert = [&h, &inserted](int v) {
        inserted.push_back(h.Insert(v, v));
    };

    auto extract = [&h, &inserted]() {
        if (inserted.empty())
            return;
        auto v = h.ExtractMin();

        auto it = std::find(inserted.begin(), inserted.end(), v.get());
        inserted.erase(it);
    };

    auto decrease = [&h, &inserted](int k, int v) {
        if (inserted.empty())
            return;
        auto it = inserted.begin();
        std::advance(it, v % inserted.size());
        auto n = *it;
        k = n->key < k ? n->key - 1 : k;
        h.DecreaseKey(n, k);
    };


    std::string line;
    int i = 0;
    while (std::getline(ss, line)) {
        std::string what, key, value;

        std::stringstream l(line);

        std::getline(l, what, ':');
        std::getline(l, key, ':');
        std::getline(l, value);

        int k = std::stoi(key);
        int v = std::stoi(value);

        switch (what[0]) {
        case 'I':
            insert(k);
            break;
        case 'D':
            decrease(k, v);
            break;
        case 'E':
            extract();
            break;
        default:
            throw;
        }
        ++i;
    }
}

#define STRING


#ifdef STRING

int main() {
    f< vh >();
    return 0;
}

#else

int main() {
    vh h;
    std::list< const N* > inserted;

    auto insert = [&h, &inserted](int v) {
        inserted.push_back(h.Insert(v, v));
    };

    auto extract = [&h, &inserted]() {
        if (inserted.empty())
            return;
        auto v = h.ExtractMin();

        auto it = std::find(inserted.begin(), inserted.end(), v.get());
        inserted.erase(it);
    };

    auto decrease = [&h, &inserted](int k, int v) {
        auto it = inserted.begin();
        std::advance(it, v % inserted.size());
        auto n = *it;
        k = n->key < k ? n->key - 1 : k;
        h.DecreaseKey(n, k);
    };

    for (int i = 0; i < 12; ++i)
        insert(i);

    decrease(-10, 13);
    extract();
    extract();
    extract();

    decrease(25, 13);

    return 0;
}

#endif
