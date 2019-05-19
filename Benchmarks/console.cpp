#include <ViolationHeap/violation_heap.hpp>
#include <FibonacciHeap/fibonacci_heap.hpp>
#include <BinomialHeap/binomial_heap.hpp>
#include <RankPairingHeap/rp_heap.hpp>

#include <list>
#include <algorithm>
#include <sstream>

using vh = MC::ViolationHeap< int >;
using fib = MC::FibonacciHeap< int >;
using bin = MC::BinomialHeap< int >;
using rp = MC::RankPairingHeap< int >;

template < typename T >
void f() {
    std::string out = "E:18:6\n"
            "E:22:10\n"
            "D:10:-24\n"
            "E:29:32\n"
            "I:-21:-1\n"
            "D:-21:17\n"
            "D:-16:-22\n"
            "E:-6:-7\n"
            "D:-30:27\n"
            "E:-25:15\n"
            "D:-4:-8\n"
            "I:13:-29\n"
            "D:-9:-20\n"
            "D:-18:-7\n"
            "I:10:-10\n"
            "D:6:25\n"
            "E:8:7\n"
            "D:14:-30\n"
            "I:-22:1\n"
            "D:17:3\n"
            "E:-17:-27\n"
            "I:6:-6\n"
            "I:6:-14\n"
            "D:-30:29\n"
            "E:21:4\n"
            "I:31:3\n"
            "D:-14:-18\n"
            "I:27:18\n"
            "I:-11:-9\n"
            "I:-30:-4\n"
            "E:-16:24\n"
            "I:27:-32\n"
            "D:-28:20\n"
            "E:-5:16\n"
            "D:12:-23\n"
            "I:-31:8\n"
            "E:-28:20\n"
            "E:13:12\n"
            "I:-4:11\n"
            "E:16:9\n"
            "E:1:17\n"
            "I:30:3\n"
            "I:-22:19\n"
            "E:-1:-17\n"
            "E:-30:1\n"
            "E:-32:12\n"
            "I:27:-31\n"
            "D:22:-32\n"
            "I:22:25\n"
            "D:1:25\n"
            "I:19:16\n"
            "E:-16:-30\n"
            "D:15:-5\n"
            "I:-20:-24\n"
            "E:-13:-28\n"
            "E:-11:-20\n"
            "I:20:16\n"
            "I:-11:28\n"
            "I:29:26\n"
            "I:16:-21\n"
            "E:11:-11\n"
            "D:9:-12\n"
            "I:10:26\n"
            "I:20:8\n"
            "E:13:27\n"
            "E:2:8\n"
            "D:1:-24\n"
            "E:15:-26\n"
            "I:-5:29\n"
            "E:28:24\n"
            "D:10:14\n"
            "E:-9:0\n"
            "D:-2:-30\n"
            "D:-23:-11\n"
            "E:4:14\n"
            "E:18:30\n"
            "I:-29:-29\n"
            "I:5:18\n"
            "E:-19:0\n"
            "I:1:7\n"
            "D:-24:15\n"
            "I:-11:0\n"
            "I:-32:-24\n"
            "I:8:-23\n"
            "I:-17:-13\n"
            "D:0:-19\n"
            "I:28:-8\n"
            "E:22:-26\n"
            "D:-22:-1\n"
            "E:-7:-25\n"
            "D:-8:1\n"
            "E:27:-30\n"
            "I:-8:10\n"
            "E:-5:5\n"
            "D:0:7\n"
            "D:-28:-23\n"
            "E:-16:1\n"
            "I:-5:22\n"
            "D:13:-27\n"
            "E:31:3";

    std::stringstream ss(out);

    T h;
    using N = typename T::NodeType;
    std::list< const N* > inserted;

    auto insert = [&h, &inserted](int k, int v) {
        inserted.push_back(h.Insert(k, v));
    };

    auto extract = [&h, &inserted](int k, int v) {
        if (h.Empty()) {
            inserted.push_back(h.Insert(k, v));
            return;
        }

        auto m = h.ExtractMin();

        auto it = std::find(inserted.begin(), inserted.end(), m.get());
        inserted.erase(it);
    };

    auto decrease = [&h, &inserted](int k, int v) {
        if (h.Empty()) {
            inserted.push_back(h.Insert(k, v));
            return;
        }

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
            insert(k, v);
            break;
        case 'D':
            decrease(k, v);
            break;
        case 'E':
            extract(k, v);
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
    f< rp >();
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
