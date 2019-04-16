#include <fstream>
#include <climits>
#include <iostream>
#include <memory>
#include <vector>
//#include "graph.hpp"
//#include "../FibonacciHeap/fibonacci_heap.hpp"
//
//void Djikstra(const std::string& file, int sourceId) {
//    graph g;
//    g.parseGraph(file);
//
//    PriorityQueues::fibonacci_heap< int, vertex > q;
//
//    std::vector< int > dist(10000, INT_MAX);
//    std::vector< int > prev(10000, -1);
//
//    dist[sourceId] = 0;
//    for (auto v : g.vertices) {
//        int id = v.first;
//        q.insert(dist[id], v.second);
//    }
//
//    while (q.count() > 0) {
//        auto n = q.minimum();
//        auto u = n->item;
//
//        for (auto i : u.neighbors) {
//            int v = i.first;
//            int d = i.second;
//
//            int alt = dist[u.id] + d;
//
//            if (alt < dist[v]) {
//                dist[v] = alt;
//                prev[v] = u.id;
//                //q.decrease_key()
//            }
//        }
//    }
//
//}

int main(int argc, char** argv) {
//    std::string file;
//    int id;
//    std::cin >> file;
//    std::cin >> id;
//    Djikstra(file, id);



    std::vector< std::unique_ptr< int > > v;

    auto f = [&v]() {
        for (int i = 0; i < 5; ++i) {
            v.emplace_back(std::make_unique< int >(i));
        }
    };

    f();

    auto where = [&v](auto invokable) {
        using T = typename decltype(v)::value_type;
        using rw = std::reference_wrapper< const T >;

        std::vector< rw > ret;
        for (const auto& p : v) {
            if (invokable(p)) {
                ret.emplace_back(std::cref(p));
            }
        }
        return ret;
    };

    v.back().reset();

    auto ret = where([](const auto& p){ return p == nullptr; });

    std::cout << ret.size();
}
